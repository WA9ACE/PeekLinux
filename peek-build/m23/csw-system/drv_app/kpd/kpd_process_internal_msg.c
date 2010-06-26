/**
 * @file   kpd_process_internal_msg.c
 *
 * Implementation of Keypad functions.
 * These functions implement the keypad processing for all the messages the
 * keypad task can receive.
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

#include "kpd/kpd_api.h"
#include "kpd/kpd_env.h"
#include "kpd/kpd_i.h"
#include "kpd/kpd_virtual_key_table_mgt.h"
#include "kpd/kpd_physical_key_def.h"
#include "kpd/kpd_cfg.h"


#include "kpd/kpd_process_internal_msg.h"

#include "rvf/rvf_api.h"
#include "rvm/rvm_use_id_list.h"

#include <string.h>

/* External declaration */
extern T_KPD_ENV_CTRL_BLK* kpd_env_ctrl_blk;
extern UINT16 kpd_configured_long_key_time;
extern UINT16 kpd_configured_repeat_key_time;


/* Definition of the wait time in the loop */
#ifdef _WINDOWS
   #define WAIT_TIME_LOOP (50)
#else
   #define WAIT_TIME_LOOP (10)
#endif


/* This structure gathers informations about one physical key Id.
   It define if subscriber is notified for this physical key */
typedef struct {  UINT32   subscriber_mask;
               }  T_PHYSICAL_KEY_MASK;


/** Definition of a set of keys with repeat keys parameters. */
typedef struct {  UINT8                nb_notified_keys;
                  UINT16               long_press_time; /* in ms */
                  UINT16               repeat_time; /* in ms */
                  T_KPD_NOTIF_LEVEL    notif_level[KPD_NB_PHYSICAL_KEYS];
               } T_PHYSICAL_KEY_PARAM_TABLE;


/* This structure gather general informations about subscriber id */
typedef struct {  T_RV_RETURN return_path;
                  T_KPD_MODE mode;
                  T_PHYSICAL_KEY_PARAM_TABLE notified_keys;
#if ((CHIPSET == 12) || (CHIPSET == 15))
                  T_KPD_TIMER_TYPE        timerchanges;
                  T_KPD_TIMEOUT_TYPE      keypad_timeout;
                  UINT16                  timeout_time;
                  T_KPD_MISS_EVENT_TYPE   miss_event;
#endif /* (CHIPSET == 12) */
               } T_SUBSCRIBER_INFOS;


/* Informations for all the physical keys Id.
   This variable is updated each time a client subscribe to the keypad driver,
   unsubscribe, or change notification key level. 
   Warn that position of the physical key Id is implicit cause of the rule
   used to define the vpm table */
static T_PHYSICAL_KEY_MASK physical_key_mask[KPD_NB_PHYSICAL_KEYS] = {0};

/* Informations for all the subscribers */
static T_SUBSCRIBER_INFOS* subscriber_infos[KPD_MAX_SUBSCRIBER] = {0};



/**
 * @name Functions implementation
 *
 */
/*@{*/

/**
 * function: kpd_return_path_already_defined
 */
static BOOL kpd_return_path_already_defined(T_RV_RETURN return_path)
{
   UINT8 i;
   BOOL ret = FALSE;

   for (i = 0; i < KPD_MAX_SUBSCRIBER; i++)
      if ( subscriber_infos[i] != 0 )
         if (subscriber_infos[i]->return_path.callback_func != 0)
         {
            if (subscriber_infos[i]->return_path.callback_func == return_path.callback_func)
            {
               ret = TRUE;
               break;
            }
         }
         else
         {
            if (subscriber_infos[i]->return_path.addr_id == return_path.addr_id)
            {
               ret = TRUE;
               break;
            }
         }

   return ret;
}

/**
 * function: kpd_subscribe_i
 */
T_RV_RET kpd_subscribe_i(T_SUBSCRIBER_ID subscriber_id,
                         T_KPD_MODE mode,
                         T_KPD_VIRTUAL_KEY_TABLE* notified_keys,
                         T_RV_RETURN return_path)
{
   INT8 i, position;
   T_RVF_MB_STATUS mb_status;
   T_RV_RET ret;

   /* Check the validity of the key table */
   ret = kpd_check_key_table(notified_keys, mode);

   if (ret != RV_OK)
   {
      /* Remove subscriber id because id was reserved */
      kpd_remove_subscriber(subscriber_id);

      /* Send error message */
      kpd_send_status_message(KPD_SUBSCRIBE_OP, KPD_ERR_KEYS_TABLE, return_path);
   }
   else
   {

      if (kpd_return_path_already_defined(return_path) == TRUE)
      {

         /* Remove subscriber id because id was reserved */
         kpd_remove_subscriber(subscriber_id);

         /* Send error message */
         kpd_send_status_message(KPD_SUBSCRIBE_OP, KPD_ERR_RETURN_PATH_EXISTING, return_path);

         ret = RV_INVALID_PARAMETER;
      }
      else
      {      
 
         /* Update subscriber informations */
         /* ------------------------------ */

         /* Reserve memory for subscriber informations */
         mb_status = rvf_get_buf (kpd_env_ctrl_blk->prim_id,
                                  sizeof(T_SUBSCRIBER_INFOS),
                                  (void **) &subscriber_infos[subscriber_id]);

         if (mb_status != RVF_RED) /* Memory allocation success */
         {
            /* Initialize structure */
            memset(subscriber_infos[subscriber_id], 0, sizeof(T_SUBSCRIBER_INFOS));

            /* Fill the subscriber structure */
            subscriber_infos[subscriber_id]->mode = mode;
            subscriber_infos[subscriber_id]->return_path = return_path;
            subscriber_infos[subscriber_id]->notified_keys.nb_notified_keys = notified_keys->nb_notified_keys;
            subscriber_infos[subscriber_id]->notified_keys.long_press_time = 0;
            subscriber_infos[subscriber_id]->notified_keys.repeat_time = 0;
            for (i = 0; i < notified_keys->nb_notified_keys; i++)
            {
               /* Retrieve physical key Id from virtual key_id */
               kpd_retrieve_virtual_key_position(  notified_keys->notified_keys[i],
                                                   mode,
                                                   &position);

               subscriber_infos[subscriber_id]->notified_keys.notif_level[position] = KPD_RELEASE_NOTIF;
            }

            /* Update link (ID <-> key) */
            /* ------------------------ */
            for (i = 0; i < notified_keys->nb_notified_keys; i++)
            {
               /* Retrieve position in vpm table */
               kpd_retrieve_virtual_key_position(notified_keys->notified_keys[i],
                                                 mode,
                                                 &position);

               /* Update subscriber mask for the physical key */
               physical_key_mask[position].subscriber_mask |= (1<<subscriber_id);

            }

            /* Send success message to suscriber */
            kpd_send_status_message(KPD_SUBSCRIBE_OP, KPD_PROCESS_OK, return_path);
         }
         else
         {
            KPD_SEND_TRACE("KPD: Memory allocation error", RV_TRACE_LEVEL_ERROR);

            /* Remove subscriber id because id was reserved */
            kpd_remove_subscriber(subscriber_id);

            /* Send error message to suscriber */
            kpd_send_status_message(KPD_SUBSCRIBE_OP, KPD_ERR_INTERNAL, return_path);
         }
      }
   }

   return RV_OK;
}


/**
 * function: kpd_unsubscribe_i
 */
T_RV_RET kpd_unsubscribe_i(T_SUBSCRIBER_ID subscriber_id)
{
   UINT8 i;

#if ((CHIPSET == 12) || (CHIPSET == 15))
   UINT8 j;
   UINT16 found = 0;
#endif

   /* Delete subscriber informations */
   rvf_free_buf(subscriber_infos[subscriber_id]);
   subscriber_infos[subscriber_id] = 0;

   /* Delete link (ID <-> key) */
   for (i = 0; i < KPD_NB_PHYSICAL_KEYS; i++)
   {
      physical_key_mask[i].subscriber_mask &= (UINT32)~(1<<subscriber_id);
   }

   /* If the subscriber is the keypad owner, this privilege is unset */
   if (kpd_is_owner_keypad(subscriber_id) == TRUE)
      kpd_set_keypad_mode(MN_MODE);

#if ((CHIPSET == 12) || (CHIPSET == 15))
    /* Are there still subscribers for repeat keys ? */

   for(i=0; i<KPD_MAX_SUBSCRIBER; i++)
   {
     if (subscriber_infos[i] != 0)
     {
       if(subscriber_infos[i]->notified_keys.nb_notified_keys > 0)
       {
          for(j=0; j < KPD_NB_PHYSICAL_KEYS; j++)
          {
            if((subscriber_infos[i]->notified_keys.notif_level[j] & KPD_INFINITE_REPEAT_NOTIF) == KPD_INFINITE_REPEAT_NOTIF)
            {
              found++;
            }
          }
       }
     }
   }

   if (found == 0)
   {
     kpd_ctrl_repeat_int(KPD_DISABLE_REPEAT);
   }
#endif

   return RV_OK;
}


/**
 * function: kpd_define_key_notification_i
 */
T_RV_RET kpd_define_key_notification_i(T_SUBSCRIBER_ID subscriber_id,
                                       T_KPD_VIRTUAL_KEY_TABLE* notif_key_table,
                                       T_KPD_NOTIF_LEVEL notif_level,
                                       UINT16 long_press_time,
                                       UINT16 repeat_time)
{
   T_RV_RET ret = RV_OK;
   UINT8 i;
   INT8 position;

   /* Check the validity of the key table */
   ret = kpd_check_key_table(notif_key_table, subscriber_infos[subscriber_id]->mode);

   if (ret != RV_OK)
   {
      /* Send error message */
      kpd_send_status_message(KPD_REPEAT_KEYS_OP, KPD_ERR_KEYS_TABLE, subscriber_infos[subscriber_id]->return_path);

   }
   else
   {
      /* Update subscriber informations */
      /* ------------------------------ */

#if ((CHIPSET == 12) || (CHIPSET == 15)) && (KPD_DECODING_MODE == HARDWARE_DECODING)
      
      if(long_press_time > 0)
      {
#if (KPD_TIME_RESOLUTION == NORMAL_RESOLUTION) 
        long_press_time *= 100; /* Convert value to mSec */       
#endif  

        long_press_time = ((long_press_time  * KPD_CLOCK_FREQ) / (1<<(KPD_CLOCK_DIVIDER+1)))-1;
        if (long_press_time != kpd_env_ctrl_blk->long_time)
        {
         /* value changed update and send msg */
          kpd_env_ctrl_blk->long_time = long_press_time;
          kpd_send_timer_changed_msg(KPD_TIMER_LONG_PRESS);
		  kpd_configured_long_key_time=long_press_time;
        }
      }
      if(repeat_time > 0)
      {
#if (KPD_TIME_RESOLUTION == NORMAL_RESOLUTION) 
        repeat_time *= 100; /* Convert value to mSec */       
#endif

        repeat_time = ((repeat_time  * KPD_CLOCK_FREQ) / (1<<(KPD_CLOCK_DIVIDER+1)))-1;
        if (repeat_time != kpd_env_ctrl_blk->repeat_time)
        {
          kpd_env_ctrl_blk->repeat_time = repeat_time;
          kpd_send_timer_changed_msg(KPD_TIMER_REPEAT);
		  kpd_configured_repeat_key_time=repeat_time;
		}
      }

#else
      subscriber_infos[subscriber_id]->notified_keys.long_press_time = long_press_time*100;
      subscriber_infos[subscriber_id]->notified_keys.repeat_time = repeat_time*100;
#endif

      for (i = 0; i < notif_key_table->nb_notified_keys; i++)
      {
         /* Retrieve physical key Id from virtual key_id */
         kpd_retrieve_virtual_key_position(  notif_key_table->notified_keys[i],
                                             subscriber_infos[subscriber_id]->mode,
                                             &position);

         /* Check if subscriber have asked notification for this key at subscription */
         if ( physical_key_mask[position].subscriber_mask & (1<<subscriber_id) )
            subscriber_infos[subscriber_id]->notified_keys.notif_level[position] = notif_level;
      }

      /* Send success message to suscriber */
      kpd_send_status_message(KPD_REPEAT_KEYS_OP, KPD_PROCESS_OK, subscriber_infos[subscriber_id]->return_path);
   }

#if ((CHIPSET == 12) || (CHIPSET == 15))
   if((notif_level & KPD_INFINITE_REPEAT_NOTIF ) == KPD_INFINITE_REPEAT_NOTIF)
   {
      /* Activate repeat interrupt */
     kpd_ctrl_repeat_int(KPD_ENABLE_REPEAT);  
   }
#endif
   return ret;
}


/**
 * function: kpd_change_mode_i
 */
T_RV_RET kpd_change_mode_i(T_SUBSCRIBER_ID subscriber_id,
                           T_KPD_VIRTUAL_KEY_TABLE* notified_keys,
                           T_KPD_MODE new_mode)
{
   UINT8 i;
   INT8 position;
   T_RV_RET ret;

   /* Check the validity of the key table */
   ret = kpd_check_key_table(notified_keys, new_mode);

   if (ret != RV_OK)
   {
      /* Send error message */
      kpd_send_status_message(KPD_CHANGE_MODE_OP, KPD_ERR_KEYS_TABLE, subscriber_infos[subscriber_id]->return_path);
   }
   else
   {
      /* Delete link (ID <-> key) for old mode*/
      for (i = 0; i < KPD_NB_PHYSICAL_KEYS; i++)
      {
         physical_key_mask[i].subscriber_mask &=(UINT32) ~(1<<subscriber_id);
         subscriber_infos[subscriber_id]->notified_keys.notif_level[i] = KPD_NO_NOTIF;
      }

      /* Update subscriber structure */
      subscriber_infos[subscriber_id]->mode = new_mode;
      subscriber_infos[subscriber_id]->notified_keys.nb_notified_keys = notified_keys->nb_notified_keys;
      subscriber_infos[subscriber_id]->notified_keys.long_press_time = 0;
      subscriber_infos[subscriber_id]->notified_keys.repeat_time = 0;
      for (i = 0; i < notified_keys->nb_notified_keys; i++)
      {
         /* Retrieve physical key Id from virtual key_id */
         kpd_retrieve_virtual_key_position(  notified_keys->notified_keys[i],
                                             new_mode,
                                             &position);

         subscriber_infos[subscriber_id]->notified_keys.notif_level[position] = KPD_RELEASE_NOTIF;
 
         /* Update link (ID <-> key) for new mode */
         physical_key_mask[position].subscriber_mask |= (1<<subscriber_id);
      }

      /* If the subscriber is the keypad owner, this privilege is unset */
      if (kpd_is_owner_keypad(subscriber_id) == TRUE)
         kpd_set_keypad_mode(MN_MODE);

      /* Send success message to suscriber */
      kpd_send_status_message(KPD_CHANGE_MODE_OP, KPD_PROCESS_OK, subscriber_infos[subscriber_id]->return_path);
   }

   return RV_OK;
}

/**
 * function: kpd_own_keypad_i
 */
T_RV_RET kpd_own_keypad_i(T_SUBSCRIBER_ID subscriber_id,
                          BOOL is_keypad_owner,
                          T_KPD_VIRTUAL_KEY_TABLE* keys_owner)
{
   INT8 position;
   UINT8 i;

   if (is_keypad_owner == FALSE)
   {
      /* Check if subscriber Id own the keypad */
      if (kpd_is_owner_keypad(subscriber_id))
      {
         kpd_set_keypad_mode(MN_MODE);

         /* Send success message to suscriber */
         kpd_send_status_message(KPD_OWN_KEYPAD_OP, KPD_PROCESS_OK, subscriber_infos[subscriber_id]->return_path);

      }
      else
         kpd_send_status_message(KPD_OWN_KEYPAD_OP, KPD_ERR_ID_OWNER_KEYPAD, subscriber_infos[subscriber_id]->return_path);
   }
   else
   {
      /* Check if keypad driver is already in single-notified mode */
      if (kpd_get_keypad_mode() == SN_MODE)
      {
         /* Send error message to suscriber */
         kpd_send_status_message(KPD_OWN_KEYPAD_OP, KPD_ERR_SN_MODE, subscriber_infos[subscriber_id]->return_path);
      }
      else
      {
         /* Check if all the keys defined in keys_owner are notified to the subsciber */
         for (i = 0; i < keys_owner->nb_notified_keys; i++)
         {
            /* Retrieve physical key Id from virtual key_id */
            kpd_retrieve_virtual_key_position(  keys_owner->notified_keys[i],
                                                subscriber_infos[subscriber_id]->mode,
                                                &position);

            if ( subscriber_infos[subscriber_id]->notified_keys.notif_level == KPD_NO_NOTIF )
            {
               /* Send error message to suscriber */
               kpd_send_status_message(KPD_OWN_KEYPAD_OP, KPD_ERR_KEYS_TABLE, subscriber_infos[subscriber_id]->return_path);
               return RV_INTERNAL_ERR;
            }
         }

         /* Set keypad driver in single-notified mode */
         kpd_set_keypad_mode(SN_MODE);

         /* Set owner keypad Id */
         kpd_set_owner_keypad_id(subscriber_id);

         /* Set list of keys used by the keypad owner. Thsi list is is a sub-list
            of the keys defined at subscription. For these keys, the keypad owner will
            be the only notified. */
         kpd_set_keys_in_sn_mode(keys_owner, subscriber_infos[subscriber_id]->mode);

         /* Send success message to suscriber */
         kpd_send_status_message(KPD_OWN_KEYPAD_OP, KPD_PROCESS_OK, subscriber_infos[subscriber_id]->return_path);
      }
   }

   return RV_OK;
}

/**
 * function: kpd_set_key_config_i
 */
T_RV_RET kpd_set_key_config_i(T_SUBSCRIBER_ID subscriber_id,
                              T_KPD_VIRTUAL_KEY_TABLE* reference_keys,
                              T_KPD_VIRTUAL_KEY_TABLE* new_keys)
{
#ifdef KPD_MODE_CONFIG
   UINT8 i;

   /* Check if some subscriber use the configurable mode */
   for (i = 0; i < KPD_MAX_SUBSCRIBER; i++)
   {
      if ( (subscriber_infos[i] != 0) && (subscriber_infos[i]->mode == KPD_MODE_CONFIG) )
      {
         /* Send error message to suscriber */
         kpd_send_status_message(KPD_SET_CONFIG_MODE_OP,
                                 KPD_ERR_CONFIG_MODE_USED,
                                 subscriber_infos[subscriber_id]->return_path);
         return RV_OK;
      }
   }

   /* Set keys in configurable mode */
   kpd_define_new_config(reference_keys, new_keys);

   /* Send success message to suscriber */
   kpd_send_status_message(KPD_SET_CONFIG_MODE_OP,
                           KPD_PROCESS_OK,
                           subscriber_infos[subscriber_id]->return_path);

#endif

   return RV_OK;
}


/**
 * function: kpd_process_key_pressed_i
 */
void kpd_process_key_pressed_i(T_KPD_PHYSICAL_KEY_ID physical_key_pressed_id)
{
   UINT8 i;
   UINT32 loop_counter = 0;
   UINT16 counter[KPD_MAX_SUBSCRIBER] = {0};
   T_KPD_PHYSICAL_KEY_ID key_id = physical_key_pressed_id;

   KPD_SEND_TRACE_PARAM("KPD: kpd_process_key_pressed_i ", key_id, RV_TRACE_LEVEL_DEBUG_LOW);

   /* Notify subscribers of the key pressed */
   for (i = 0; i < KPD_MAX_SUBSCRIBER; i++) /* To do : Loop on the real number of subscribers */
   { /* To do : Test on the physical_key_mask (to ensure subscriber is subscribed for this key) */
      if ( (subscriber_infos[i]!=0) && (subscriber_infos[i]->notified_keys.notif_level[key_id] & KPD_FIRST_PRESS_NOTIF) )
      {
         kpd_send_key_event_message(key_id, KPD_KEY_PRESSED,
                                    KPD_FIRST_PRESS, subscriber_infos[i]->mode,
                                    subscriber_infos[i]->return_path);
      }
   }

   /* If key pressed is the PWR key, the message "Released key" is immediately sent */
   if (key_id != KPD_SHORT_PRESS_PWR_KEY)
   {
      /* Loop infinitely until key is released */
      do
      {
         rvf_delay(RVF_MS_TO_TICKS(WAIT_TIME_LOOP));
         physical_key_pressed_id = kpd_scan_keypad();
         loop_counter++;

         /* Send message for repeat key */
         for (i = 0; i < KPD_MAX_SUBSCRIBER; i++)
         {
            if ( (subscriber_infos[i]!=0) && (subscriber_infos[i]->notified_keys.notif_level[key_id] & (KPD_LONG_PRESS_NOTIF|KPD_INFINITE_REPEAT_NOTIF)) )
            {
               if ((counter[i] == 0) && (WAIT_TIME_LOOP*loop_counter >= subscriber_infos[i]->notified_keys.long_press_time) )
               {
                  kpd_send_key_event_message(key_id, KPD_KEY_PRESSED,
                                             KPD_LONG_PRESS, subscriber_infos[i]->mode,
                                             subscriber_infos[i]->return_path);
                  counter[i] ++;
               }
               else if (subscriber_infos[i]->notified_keys.notif_level[key_id] & KPD_INFINITE_REPEAT_NOTIF)
               {
                  if (WAIT_TIME_LOOP*loop_counter >= (UINT32)((counter[i]*subscriber_infos[i]->notified_keys.repeat_time + subscriber_infos[i]->notified_keys.long_press_time)) )
                  {
                     kpd_send_key_event_message(key_id, KPD_KEY_PRESSED,
                                                KPD_REPEAT_PRESS, subscriber_infos[i]->mode,
                                                subscriber_infos[i]->return_path);
                     counter[i] ++;
                  }
               }
            }
         }

      } while (physical_key_pressed_id != KPD_PKEY_NULL);
   }

   /* Notify subscribers of the key released */
   for (i = 0; i < KPD_MAX_SUBSCRIBER; i++)
   {
      if ( (subscriber_infos[i]!=0) && (subscriber_infos[i]->notified_keys.notif_level[key_id] & KPD_RELEASE_NOTIF) )
      {
         kpd_send_key_event_message(key_id, KPD_KEY_RELEASED,
                                    KPD_INSIGNIFICANT_VALUE, subscriber_infos[i]->mode,
                                    subscriber_infos[i]->return_path);
      }
   }

   /* On board,this function unmask keypad interrupt
      On Riviera tool, this function is empty        */
   if (key_id != KPD_SHORT_PRESS_PWR_KEY)
   {

#if ((CHIPSET == 12) || (CHIPSET == 15))
		kpd_software_reset();
      kpd_init_ctrl_reg(1, HARDWARE_DECODING, KPD_CLK_DIV32,
                        KPD_DETECTION_DISABLED, KPD_DETECTION_DISABLED,
                        KPD_DETECTION_DISABLED, KPD_DETECTION_DISABLED);
#endif

      kpd_acknowledge_key_pressed();
   }
}


/**
 * function: kpd_process_key_pressed_sn_mode_i
 */
void kpd_process_key_pressed_sn_mode_i(T_KPD_PHYSICAL_KEY_ID physical_key_pressed_id)
{
   T_SUBSCRIBER_ID owner_keypad_id = kpd_get_owner_keypad_id();
   T_KPD_PHYSICAL_KEY_ID key_id = physical_key_pressed_id;
   UINT32 loop_counter = 0;
   UINT16 counter = 0;

   if ( subscriber_infos[owner_keypad_id]->notified_keys.notif_level[key_id] & KPD_FIRST_PRESS_NOTIF )
   {
      /* Notify subscribers of the key pressed */
       kpd_send_key_event_message(key_id, KPD_KEY_PRESSED,
                                  KPD_FIRST_PRESS, subscriber_infos[owner_keypad_id]->mode,
                                  subscriber_infos[owner_keypad_id]->return_path);
   }

   /* If key pressed is the PWR key, the message "Released key" is immediately sent */
   if (key_id != KPD_SHORT_PRESS_PWR_KEY)
   {
      /* Loop infinitely until key is released */
      do
      {
         rvf_delay(RVF_MS_TO_TICKS(WAIT_TIME_LOOP));
         physical_key_pressed_id = kpd_scan_keypad();
         loop_counter++;

         /* Send message for repeat key */
         if (subscriber_infos[owner_keypad_id]->notified_keys.notif_level[key_id] & (KPD_LONG_PRESS_NOTIF|KPD_INFINITE_REPEAT_NOTIF))
         {
            if ((counter == 0) && (WAIT_TIME_LOOP*loop_counter >= subscriber_infos[owner_keypad_id]->notified_keys.long_press_time) )
            {
               kpd_send_key_event_message(key_id, KPD_KEY_PRESSED,
                                          KPD_LONG_PRESS, subscriber_infos[owner_keypad_id]->mode,
                                          subscriber_infos[owner_keypad_id]->return_path);
               counter ++;
            }
            else if (subscriber_infos[owner_keypad_id]->notified_keys.notif_level[key_id] & KPD_INFINITE_REPEAT_NOTIF)
            {
               if (WAIT_TIME_LOOP*loop_counter >= (UINT32)((counter*subscriber_infos[owner_keypad_id]->notified_keys.repeat_time + subscriber_infos[owner_keypad_id]->notified_keys.long_press_time)) )
               {
                  kpd_send_key_event_message(key_id, KPD_KEY_PRESSED,
                                             KPD_REPEAT_PRESS, subscriber_infos[owner_keypad_id]->mode,
                                             subscriber_infos[owner_keypad_id]->return_path);
                  counter ++;
               }
            }
         }

      } while (physical_key_pressed_id != KPD_PKEY_NULL);
   }

   if ( subscriber_infos[owner_keypad_id]->notified_keys.notif_level[key_id] & KPD_RELEASE_NOTIF )
   {
      /* Notify subscribers of the key released */
      kpd_send_key_event_message(key_id, KPD_KEY_RELEASED,
                                 KPD_INSIGNIFICANT_VALUE, subscriber_infos[owner_keypad_id]->mode,
                                 subscriber_infos[owner_keypad_id]->return_path);
   }

   /* On board,this function unmask keypad interrupt
      On Riviera tool, this function authorize to send new messages to keypad task */
   if (key_id != KPD_SHORT_PRESS_PWR_KEY)
   {
#if ((CHIPSET == 12) || (CHIPSET == 15))
      kpd_software_reset();
      kpd_init_ctrl_reg(1, HARDWARE_DECODING, KPD_CLK_DIV32,
                        KPD_DETECTION_DISABLED, KPD_DETECTION_DISABLED,
                        KPD_DETECTION_DISABLED, KPD_DETECTION_DISABLED);
#endif
      kpd_acknowledge_key_pressed();
   }
}

/**
 * function: kpd_wait_for_key_release
 */
void kpd_wait_for_key_release(void)
{
   T_KPD_PHYSICAL_KEY_ID key_id;;

   do
   {
      rvf_delay(RVF_MS_TO_TICKS(WAIT_TIME_LOOP));
      key_id = kpd_scan_keypad();

   } while (key_id != KPD_PKEY_NULL);

   kpd_acknowledge_key_pressed();
}

#if ((CHIPSET == 12) || (CHIPSET == 15))

/**
 * function: kpd_set_timerchanges_i
 */
void kpd_set_timerchanges_i(T_SUBSCRIBER_ID subscriber_id,
                            T_KPD_TIMER_TYPE timer_type)
{
  subscriber_infos[subscriber_id]->timerchanges = timer_type;
}

/**
 * function: kpd_set_keypad_timeout_i
 */
void kpd_set_keypad_timeout_i(T_SUBSCRIBER_ID subscriber_id,
                              T_KPD_TIMEOUT_TYPE  timeout_type,
                              UINT16 timeout_time)

{
  /* Set desired timeout type in subscribers info struct */
  subscriber_infos[subscriber_id]->keypad_timeout = timeout_type;

  /* Set time out in hardware register */
  kpd_set_timeout_time(timeout_time);
}

/**
 * function: kpd_set_miss_event_i
 */
void kpd_set_miss_event_i(T_SUBSCRIBER_ID subscriber_id,
                              T_KPD_MISS_EVENT_TYPE missevent_type)
{
  subscriber_infos[subscriber_id]->miss_event = missevent_type;
}


/**
 * function: kpd_process_timeout_event_i
 */
void kpd_process_timeout_event_i(void)
{
  UINT8 i;
  T_RVF_MB_STATUS mb_status;
  T_KPD_KEYPAD_TIMEOUT_MSG* keypad_timeout_msg;
  T_KPD_TIMEOUT_TYPE timeout_type;


  /* Check if a key was previously pressed */
  if (kpd_env_ctrl_blk->nb_active_keys == 0)
  {
    timeout_type = KPD_TIMEOUT_NO_KEY;  
  }
  else
  {
    timeout_type = KPD_TIMEOUT_KEY_JAM;
  }

  /* Send the clients that subscribed to timeout event a message */
  for (i = 0; i < KPD_MAX_SUBSCRIBER; i++)
  {
      if(subscriber_infos[i] != 0)
      {
        /* Is client registered */
        if (subscriber_infos[i]->keypad_timeout & timeout_type)
        {
				    /* Create buffer for message */
          mb_status = rvf_get_buf (kpd_env_ctrl_blk->prim_id, sizeof(T_KPD_KEYPAD_TIMEOUT_MSG), 
						                                                  (void **) &keypad_timeout_msg);
		  if (mb_status != RVF_RED)
      	  {
              keypad_timeout_msg->hdr.msg_id = KPD_KEYPAD_TIMEOUT_MSG;
              keypad_timeout_msg->timeout_type = timeout_type;
		      if (subscriber_infos[i]->return_path.callback_func != 0)
              {
                 subscriber_infos[i]->return_path.callback_func((void*) keypad_timeout_msg);
                 rvf_free_buf(keypad_timeout_msg);
              }
              else
              {
                 rvf_send_msg(subscriber_infos[i]->return_path.addr_id, keypad_timeout_msg);
              }

              KPD_SEND_TRACE("KPD: Send 'keypad timeout message", RV_TRACE_LEVEL_DEBUG_LOW);
		  
		  }
  		  else
		  {
             KPD_SEND_TRACE("KPD: Memory allocation error", RV_TRACE_LEVEL_ERROR);
          }
        }
	  }
  }
}

/*!venkat*/
 #if 0
    /* Create buffer for message */
    mb_status = rvf_get_buf (kpd_env_ctrl_blk->prim_id, sizeof(T_KPD_KEYPAD_TIMEOUT_MSG), (void **) &keypad_timeout_msg);

    /* Check status of memory allocation */
    if (mb_status == RVF_RED)
    {
      KPD_SEND_TRACE("KPD: Memory allocation error", RV_TRACE_LEVEL_ERROR);
    }
    else
    {
      /* Client active ? */
      if(subscriber_infos[i] != 0)
      {
        /* Is client registered */
        if (subscriber_infos[i]->keypad_timeout & timeout_type)
        {
          keypad_timeout_msg->hdr.msg_id = KPD_KEYPAD_TIMEOUT_MSG;
          keypad_timeout_msg->timeout_type = timeout_type;
        }
        else
        {
          /* Free claimed memory */
          rvf_free_buf(keypad_timeout_msg);
          keypad_timeout_msg = NULL;
        }

        if (keypad_timeout_msg != NULL)
        {
          /* Send message to the client */
          if (subscriber_infos[i]->return_path.callback_func != 0)
          {
            subscriber_infos[i]->return_path.callback_func((void*) keypad_timeout_msg);
            rvf_free_buf(keypad_timeout_msg);
          }
          else
          {
            rvf_send_msg(subscriber_infos[i]->return_path.addr_id, keypad_timeout_msg);
          }

          KPD_SEND_TRACE("KPD: Send 'keypad timeout message", RV_TRACE_LEVEL_DEBUG_LOW);
        }
      }
    }
  }
}

#endif //#if 0

/**
 * function: kpd_process_miss_event_i
 */
void kpd_process_miss_event_i(void)
{
  UINT8 i;
  T_RVF_MB_STATUS mb_status;
  T_KPD_KEYPAD_MISS_EVENT_MSG* keypad_miss_event_msg;


  for (i = 0; i < KPD_MAX_SUBSCRIBER; i++)
  {
    /* Client active ? */
    if(subscriber_infos[i] != 0)
    {
      /* Is client registered */
      if ((subscriber_infos[i]->miss_event & KPD_MISS_EVENT_ON) == KPD_MISS_EVENT_ON)
      {
        /* Create buffer for message */
        mb_status = rvf_get_buf (kpd_env_ctrl_blk->prim_id, sizeof(T_KPD_KEYPAD_MISS_EVENT_MSG), (void **) &keypad_miss_event_msg);

        if (mb_status != RVF_RED)
        {
          keypad_miss_event_msg->hdr.msg_id = KPD_KEYPAD_MISS_EVENT_MSG;

          /* Send message to the client */
          if (subscriber_infos[i]->return_path.callback_func != 0)
          {
            subscriber_infos[i]->return_path.callback_func((void*) keypad_miss_event_msg);
            rvf_free_buf(keypad_miss_event_msg);
          }
          else
          {
            rvf_send_msg(subscriber_infos[i]->return_path.addr_id, keypad_miss_event_msg);
          }

          KPD_SEND_TRACE("KPD: Send 'keypad miss event message", RV_TRACE_LEVEL_DEBUG_LOW);

        }
        else
        {
          KPD_SEND_TRACE("KPD: Memory allocation error", RV_TRACE_LEVEL_ERROR);
        }
      }
    }
  }
}


/**
 * Function : kpd_process_key_event
 */
void kpd_process_key_event_i(UINT8 nb_keys_pressed, INT8* key_pressed_id)
{
  UINT8   i,j;
  INT8   key_id;
 
  /* Update received key table */
  kpd_update_received_key_info(nb_keys_pressed, key_pressed_id);

  for(i=0; i<kpd_env_ctrl_blk->nb_active_keys; i++)
  {
    key_id = kpd_env_ctrl_blk->received_key_info[i].key;
    
    if(kpd_env_ctrl_blk->received_key_info[i].key_state == first_press)
    {
      /* Check if the key is in SN mode */
      if(kpd_is_key_in_sn_mode(key_id) == TRUE)
      {
        T_SUBSCRIBER_ID owner_keypad_id = kpd_get_owner_keypad_id();

        if ( subscriber_infos[owner_keypad_id]->notified_keys.notif_level[key_id] & KPD_FIRST_PRESS_NOTIF )
        {
          /* Notify subscriber of the key pressed */
          kpd_send_key_event_message(key_id, KPD_KEY_PRESSED,
                                     KPD_FIRST_PRESS, subscriber_infos[owner_keypad_id]->mode,
                                     subscriber_infos[owner_keypad_id]->return_path);
        }
      }
      else
      {
        /* Notify subscribers of the key pressed */
        for (j = 0; j < KPD_MAX_SUBSCRIBER; j++) 
        { 
         if ( (subscriber_infos[j]!=0)  && (subscriber_infos[j]->notified_keys.notif_level[key_id] & KPD_FIRST_PRESS_NOTIF) )
          {
            kpd_send_key_event_message(key_id, KPD_KEY_PRESSED,
                                       KPD_FIRST_PRESS, subscriber_infos[j]->mode,
                                       subscriber_infos[j]->return_path);
          }
        }
      }
    }
    else if (kpd_env_ctrl_blk->received_key_info[i].key_state == released)
    {
      /* Check if the key is in SN mode */
      if(kpd_is_key_in_sn_mode(key_id) == TRUE)
      {
        T_SUBSCRIBER_ID owner_keypad_id = kpd_get_owner_keypad_id();

        if ( subscriber_infos[owner_keypad_id]->notified_keys.notif_level[key_id] & KPD_RELEASE_NOTIF )
        {
          /* Notify subscriber of the key pressed */
          kpd_send_key_event_message(key_id, KPD_KEY_RELEASED,
                                     KPD_INSIGNIFICANT_VALUE, subscriber_infos[owner_keypad_id]->mode,
                                     subscriber_infos[owner_keypad_id]->return_path);
        }
      }
      else
      {
        /* Notify subscribers of the key pressed */
        for (j = 0; j < KPD_MAX_SUBSCRIBER; j++) 
        { 
          if ( (subscriber_infos[j]!=0) && (subscriber_infos[j]->notified_keys.notif_level[key_id] & KPD_RELEASE_NOTIF) )
          {
            kpd_send_key_event_message(key_id, KPD_KEY_RELEASED,
                                       KPD_INSIGNIFICANT_VALUE, subscriber_infos[j]->mode,
                                       subscriber_infos[j]->return_path);
          }
        }
      }
    }
  }
  kpd_remove_released_from_key_info();
}


/**
 * function: kpd_process_long_key_i
 */
void kpd_process_long_key_i(UINT8 nb_keys_pressed, INT8* key_pressed_id)
{
  UINT8   i,j;
  INT8   key_id;

    kpd_process_long_key_received_key_info(nb_keys_pressed, key_pressed_id);

    /* send messages to subscribers */
  for(i=0; i<kpd_env_ctrl_blk->nb_active_keys; i++)
  {
    key_id = kpd_env_ctrl_blk->received_key_info[i].key;
    
    if(kpd_env_ctrl_blk->received_key_info[i].key_state == long_press)
    {
      /* Check if the key is in SN mode */
      if(kpd_is_key_in_sn_mode(key_id) == TRUE)
      {
        T_SUBSCRIBER_ID owner_keypad_id = kpd_get_owner_keypad_id();

        if ( subscriber_infos[owner_keypad_id]->notified_keys.notif_level[key_id] & KPD_LONG_PRESS_NOTIF )
        {
          /* Notify subscriber of the key pressed */
          kpd_send_key_event_message(key_id, KPD_KEY_PRESSED,
                                     KPD_LONG_PRESS, subscriber_infos[owner_keypad_id]->mode,
                                     subscriber_infos[owner_keypad_id]->return_path);
        }
      }
      else
      {
        /* Notify subscribers of the key pressed */
        for (j = 0; j < KPD_MAX_SUBSCRIBER; j++) 
        { 
         if ( (subscriber_infos[j]!=0)  && (subscriber_infos[j]->notified_keys.notif_level[key_id] & KPD_LONG_PRESS_NOTIF) )
          {
            kpd_send_key_event_message(key_id, KPD_KEY_PRESSED,
                                       KPD_LONG_PRESS, subscriber_infos[j]->mode,
                                       subscriber_infos[j]->return_path);
          }
        }
      }
    }
    else if (kpd_env_ctrl_blk->received_key_info[i].key_state == repeat_press)
    {
       /* Check if the key is in SN mode */
      if(kpd_is_key_in_sn_mode(key_id) == TRUE)
      {
        T_SUBSCRIBER_ID owner_keypad_id = kpd_get_owner_keypad_id();

        if ( subscriber_infos[owner_keypad_id]->notified_keys.notif_level[key_id] & KPD_INFINITE_REPEAT_NOTIF )
        {
          /* Notify subscriber of the key pressed */
          kpd_send_key_event_message(key_id, KPD_KEY_PRESSED,
                                     KPD_REPEAT_PRESS, subscriber_infos[owner_keypad_id]->mode,
                                     subscriber_infos[owner_keypad_id]->return_path);
        }
      }
      else
      {
        /* Notify subscribers of the key pressed */
        for (j = 0; j < KPD_MAX_SUBSCRIBER; j++) 
        { 
         if ( (subscriber_infos[j]!=0)  && (subscriber_infos[j]->notified_keys.notif_level[key_id] & KPD_INFINITE_REPEAT_NOTIF) )
          {
            kpd_send_key_event_message(key_id, KPD_KEY_PRESSED,
                                       KPD_REPEAT_PRESS, subscriber_infos[j]->mode,
                                       subscriber_infos[j]->return_path);
          }
        }
      }
    }
  }   
}


/**
 * function: kpd_send_timer_changed_msg
 */

void kpd_send_timer_changed_msg(T_KPD_TIMER_TYPE timer_type)
{

  UINT8 i;
  T_RVF_MB_STATUS mb_status;
  T_KPD_TIMER_CHANGED_MSG* timer_changed_msg;

  for (i = 0; i < KPD_MAX_SUBSCRIBER; i++)
  {
    /* Client active ? */
    if(subscriber_infos[i] != 0)
    {
      /* Is client registered */
      if ((subscriber_infos[i]->timerchanges & timer_type) == timer_type) 
      {

        /* Create buffer for message */
        mb_status = rvf_get_buf (kpd_env_ctrl_blk->prim_id, sizeof(T_KPD_TIMER_CHANGED_MSG), (void **) &timer_changed_msg);

        if (mb_status != RVF_RED)
        {

          timer_changed_msg->hdr.msg_id = KPD_TIMER_CHANGED_MSG;

          timer_changed_msg->timer_type = timer_type;

          /* Send message to the client */
          if (subscriber_infos[i]->return_path.callback_func != 0)
          {
            subscriber_infos[i]->return_path.callback_func((void*) timer_changed_msg);
            rvf_free_buf(timer_changed_msg);
          }
          else
          {
            rvf_send_msg(subscriber_infos[i]->return_path.addr_id, timer_changed_msg);
          }

          KPD_SEND_TRACE("KPD: Send 'timer_changed_msg'", RV_TRACE_LEVEL_DEBUG_LOW);
        }
        else
        {
          KPD_SEND_TRACE("KPD: Memory allocation error", RV_TRACE_LEVEL_ERROR);
        }
      }
    }
  }
}

/**
 * function : clr_subscriber_table
 */
void kpd_clr_subscriber_table(void)
{
  UINT8 i;

  for(i = 0; i < KPD_MAX_SUBSCRIBER; i++)
  {
    subscriber_infos[i] = 0;
  }
}

#endif /* (CHIPSET == 12) */



/*@}*/
