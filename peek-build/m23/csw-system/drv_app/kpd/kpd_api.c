/**
 * @file   kpd_api.c
 *
 * Implementation of bridge functions.
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
#include "kpd/kpd_virtual_key_table_mgt.h"
#include "kpd/kpd_messages_i.h"
#include "kpd/kpd_process_internal_msg.h"
#include "kpd/kpd_env.h"

#include "rvm/rvm_use_id_list.h"

/* Include file to delete when Kp global variable will be useless */
/* Delete variable Kp below */
#include "kpd/kpd_power_api.h"


/** External declaration */
extern T_KPD_ENV_CTRL_BLK* kpd_env_ctrl_blk;
extern T_KPD_KEYPAD Kp;


/**
 * @name Bridge functions implementation
 *
 */
/*@{*/


/**
 * function: kpd_subscribe
 */
T_RV_RET kpd_subscribe(T_KPD_SUBSCRIBER* subscriber_p,
                       T_KPD_MODE mode,
                       T_KPD_VIRTUAL_KEY_TABLE* notified_keys_p,
                       T_RV_RETURN return_path)
{
   T_RVF_MB_STATUS mb_status;
   T_KPD_SUBSCRIBE_MSG* msg_subscribe_p;
   T_SUBSCRIBER* subscriber_struct_p;
   T_RV_RET ret = RV_OK;
   UINT8 i;

   /* Initialization of parameter "subscriber" in order to be sure that client will
      not use later the parameter with an non initialized value */
   *subscriber_p = 0;

   /* Check if initialization has been correctly done */
   if ( (kpd_env_ctrl_blk == 0) || (kpd_env_ctrl_blk->swe_is_initialized == FALSE) )
   {
      KPD_SEND_TRACE("KPD: Initialization is not yet done or failed", RV_TRACE_LEVEL_ERROR);
      return RV_INTERNAL_ERR;
   }

   /* Allocate memory to save subscriber Id */
   mb_status = rvf_get_buf (kpd_env_ctrl_blk->prim_id, sizeof(T_SUBSCRIBER), (void **) &subscriber_struct_p);   
   if (mb_status == RVF_RED)
      return RV_MEMORY_ERR;

   if ( (notified_keys_p != 0)
      && (notified_keys_p->nb_notified_keys > 0)
      && (notified_keys_p->nb_notified_keys <= KPD_NB_PHYSICAL_KEYS) )
   {
      /* Reserve subscriber Id */
      ret = kpd_add_subscriber(&(subscriber_struct_p->subscriber_id));
      if (ret != RV_OK)
         rvf_free_buf(subscriber_struct_p);
   }
   else
   {
      rvf_free_buf(subscriber_struct_p);
      ret = RV_INVALID_PARAMETER;
   }


   if ((ret == RV_OK) && (notified_keys_p != 0) )
   {
      /* Reserve memory for message */
      mb_status = rvf_get_buf (kpd_env_ctrl_blk->prim_id, sizeof(T_KPD_SUBSCRIBE_MSG), (void **) &msg_subscribe_p);   

      if (mb_status != RVF_RED) /* Memory allocation success */
      {
         /* Fill the message */
         msg_subscribe_p->hdr.msg_id = KPD_SUBSCRIBE_MSG;
         msg_subscribe_p->subscription_info.subscriber_id = subscriber_struct_p->subscriber_id;
         msg_subscribe_p->subscription_info.mode = mode;
         msg_subscribe_p->subscription_info.return_path = return_path;
         msg_subscribe_p->subscription_info.notified_keys.nb_notified_keys = notified_keys_p->nb_notified_keys;
         for (i = 0; i < notified_keys_p->nb_notified_keys; i++)
            msg_subscribe_p->subscription_info.notified_keys.notified_keys[i] = notified_keys_p->notified_keys[i];

         /* Send message to the keypad task */
         rvf_send_msg(kpd_env_ctrl_blk->addr_id, msg_subscribe_p);

         /* Save subscriber id */
         *subscriber_p = (void*)subscriber_struct_p;
      }
      else
      {
         KPD_SEND_TRACE("KPD: Memory allocation error", RV_TRACE_LEVEL_ERROR);
         kpd_remove_subscriber(subscriber_struct_p->subscriber_id);
         rvf_free_buf(subscriber_struct_p);
         ret = RV_MEMORY_ERR;
      }
   }

   return ret;
}


/**
 * function: kpd_unsubscribe
 */
T_RV_RET kpd_unsubscribe(T_KPD_SUBSCRIBER* subscriber_p)
{
   T_RVF_MB_STATUS mb_status;
   T_KPD_UNSUBSCRIBE_MSG* msg_unsubscribe_p;
   T_RV_RET ret = RV_INVALID_PARAMETER;
   T_SUBSCRIBER_ID subscriber_id;

   /* Check if subscriber id is correct */
   if ( kpd_subscriber_id_used(*subscriber_p, &subscriber_id) == TRUE)
   {
      /* Reserve memory for message */
      mb_status = rvf_get_buf (kpd_env_ctrl_blk->prim_id, sizeof(T_KPD_UNSUBSCRIBE_MSG), (void **) &msg_unsubscribe_p);   

      if (mb_status != RVF_RED) /* Memory allocation success */
      {
         /* Free subscriber Id */
         ret = kpd_remove_subscriber(subscriber_id);

         if (ret == RV_OK)
         {
            /* Fill the message */
            msg_unsubscribe_p->hdr.msg_id = KPD_UNSUBSCRIBE_MSG;
            msg_unsubscribe_p->subscriber_id = subscriber_id;

            /* Send message to the keypad task */
            rvf_send_msg(kpd_env_ctrl_blk->addr_id, msg_unsubscribe_p);

            rvf_free_buf(*subscriber_p);
            *subscriber_p = 0;
         }
         else
         {
            rvf_free_buf(msg_unsubscribe_p);
         }
      }
      else
      {
         KPD_SEND_TRACE("KPD: Memory allocation error", RV_TRACE_LEVEL_ERROR);
         ret = RV_MEMORY_ERR;
      }
   }

   return ret;
}


/**
 * function: kpd_define_key_notification
 */
T_RV_RET kpd_define_key_notification(T_KPD_SUBSCRIBER subscriber,
                                     T_KPD_VIRTUAL_KEY_TABLE* notif_key_table_p,
                                     T_KPD_NOTIF_LEVEL notif_level,
                                     UINT16 long_press_time,
                                     UINT16 repeat_time)
{
   T_RV_RET ret = RV_INVALID_PARAMETER;
   T_RVF_MB_STATUS mb_status;
   T_KPD_NOTIF_KEYS_MSG* msg_notif_key_p;
   UINT8 i;
   T_SUBSCRIBER_ID subscriber_id;

   /* Check if subscriber id is correct */
   if ( kpd_subscriber_id_used(subscriber, &subscriber_id) == TRUE)
   {
      if ( (notif_key_table_p != 0)
         && (notif_key_table_p->nb_notified_keys > 0)
         && (notif_key_table_p->nb_notified_keys <= KPD_NB_PHYSICAL_KEYS) )
      {
         if ( (notif_level == KPD_NO_NOTIF )
            ||(notif_level & KPD_FIRST_PRESS_NOTIF )
            ||(notif_level & KPD_RELEASE_NOTIF )
            ||( (notif_level & KPD_LONG_PRESS_NOTIF) && (long_press_time != 0) )
            ||( (notif_level & KPD_INFINITE_REPEAT_NOTIF) && (long_press_time != 0) && (repeat_time != 0) ) )
         {
            /* Reserve memory for message */
            mb_status = rvf_get_buf (kpd_env_ctrl_blk->prim_id, sizeof(T_KPD_NOTIF_KEYS_MSG), (void **) &msg_notif_key_p);   

            if (mb_status != RVF_RED) /* Memory allocation success */
            {
               /* Fill the message */
               msg_notif_key_p->hdr.msg_id = KPD_NOTIF_KEYS_MSG;
               msg_notif_key_p->subscriber_id = subscriber_id;
               msg_notif_key_p->notif_level = notif_level;
               msg_notif_key_p->long_press_time = long_press_time;
               msg_notif_key_p->repeat_time = repeat_time;
               msg_notif_key_p->notif_key_table.nb_notified_keys = notif_key_table_p->nb_notified_keys;
               for (i = 0; i < notif_key_table_p->nb_notified_keys; i++)
                  msg_notif_key_p->notif_key_table.notified_keys[i] = notif_key_table_p->notified_keys[i];

               /* Send message to the keypad task */
               rvf_send_msg(kpd_env_ctrl_blk->addr_id, msg_notif_key_p);

               ret = RV_OK;
            }
            else
            {
               KPD_SEND_TRACE("KPD: Memory allocation error", RV_TRACE_LEVEL_ERROR);
               ret = RV_MEMORY_ERR;
            }
         }
      }
   }

   return ret;
}


/**
 * function: kpd_change_mode
 */
T_RV_RET kpd_change_mode( T_KPD_SUBSCRIBER subscriber,
                          T_KPD_VIRTUAL_KEY_TABLE* notified_keys_p,
                          T_KPD_MODE new_mode)
{
   T_RV_RET ret = RV_INVALID_PARAMETER;
   T_RVF_MB_STATUS mb_status;
   T_KPD_CHANGE_MODE_MSG* msg_change_mode_p;
   T_SUBSCRIBER_ID subscriber_id;
   UINT8 i;

   /* Check if subscriber id is correct */
   if ( kpd_subscriber_id_used(subscriber, &subscriber_id) == TRUE)
   {
      if ( (notified_keys_p != 0)
         && (notified_keys_p->nb_notified_keys > 0)
         && (notified_keys_p->nb_notified_keys <= KPD_NB_PHYSICAL_KEYS) )
      {
         /* Reserve memory for message */
         mb_status = rvf_get_buf (kpd_env_ctrl_blk->prim_id, sizeof(T_KPD_CHANGE_MODE_MSG), (void **) &msg_change_mode_p);   

         if (mb_status != RVF_RED) /* Memory allocation success */
         {
            /* Fill the message */
            msg_change_mode_p->hdr.msg_id = KPD_CHANGE_MODE_MSG;
            msg_change_mode_p->subscriber_id = subscriber_id;
            msg_change_mode_p->new_mode = new_mode;
            msg_change_mode_p->notified_keys.nb_notified_keys = notified_keys_p->nb_notified_keys;
            for (i = 0; i < notified_keys_p->nb_notified_keys; i++)
               msg_change_mode_p->notified_keys.notified_keys[i] = notified_keys_p->notified_keys[i];

            /* Send message to the keypad task */
            rvf_send_msg(kpd_env_ctrl_blk->addr_id, msg_change_mode_p);

            ret = RV_OK;
         }
         else
         {
            KPD_SEND_TRACE("KPD: Memory allocation error", RV_TRACE_LEVEL_ERROR);
            ret = RV_MEMORY_ERR;
         }
      }
   }

   return ret;
}

/**
 * function: kpd_own_keypad
 */
T_RV_RET kpd_own_keypad( T_KPD_SUBSCRIBER subscriber,
                         BOOL is_keypad_owner,
                         T_KPD_VIRTUAL_KEY_TABLE* keys_owner_p)
{
   T_RV_RET ret = RV_INVALID_PARAMETER;
   T_RVF_MB_STATUS mb_status;
   T_KPD_OWN_KEYPAD_MSG* msg_own_keypad_p;
   T_SUBSCRIBER_ID subscriber_id;
   UINT8 i;

   /* Check if subscriber id is correct */
   if ( kpd_subscriber_id_used(subscriber, &subscriber_id) == TRUE)
   {
      /* If subscriber want to be the owner of the keypad, list of keys is checked
         else, subscriber want to release the keypad, check list of key is useless */ 
      if (is_keypad_owner == TRUE)
      {
         if ( (keys_owner_p != 0)
            && (keys_owner_p->nb_notified_keys > 0)
            && (keys_owner_p->nb_notified_keys <= KPD_NB_PHYSICAL_KEYS) )
            ret = RV_OK;
      }
      else
         ret = RV_OK;

      if ((ret == RV_OK) && (keys_owner_p != 0))
      {
         /* Reserve memory for message */
         mb_status = rvf_get_buf (kpd_env_ctrl_blk->prim_id, sizeof(T_KPD_OWN_KEYPAD_MSG), (void **) &msg_own_keypad_p);   

         if (mb_status != RVF_RED) /* Memory allocation success */
         {
            /* Fill the message */
            msg_own_keypad_p->hdr.msg_id = KPD_OWN_KEYPAD_MSG;
            msg_own_keypad_p->subscriber_id = subscriber_id;
            msg_own_keypad_p->is_keypad_owner = is_keypad_owner;
            msg_own_keypad_p->keys_owner.nb_notified_keys = keys_owner_p->nb_notified_keys;
            for (i = 0; i < keys_owner_p->nb_notified_keys; i++)
               msg_own_keypad_p->keys_owner.notified_keys[i] = keys_owner_p->notified_keys[i];

            /* Send message to the keypad task */
            rvf_send_msg(kpd_env_ctrl_blk->addr_id, msg_own_keypad_p);
         }
         else
         {
            KPD_SEND_TRACE("KPD: Memory allocation error", RV_TRACE_LEVEL_ERROR);
            ret = RV_MEMORY_ERR;
         }
       }
   }

   return ret;
}

/**
 * function: kpd_set_key_config
 */
T_RV_RET kpd_set_key_config(T_KPD_SUBSCRIBER subscriber,
                            T_KPD_VIRTUAL_KEY_TABLE* reference_keys_p,
                            T_KPD_VIRTUAL_KEY_TABLE* new_keys_p)
{
#ifdef KPD_MODE_CONFIG
   T_RVF_MB_STATUS mb_status;
   T_KPD_SET_CONFIG_MODE_MSG* msg_set_config_mode_p;
   T_RV_RET ret = RV_INVALID_PARAMETER;
   T_SUBSCRIBER_ID subscriber_id;
   UINT8 i;
   INT8 position;

   /* Check if subscriber id is correct */
   if ( kpd_subscriber_id_used(subscriber, &subscriber_id) == TRUE)
   {
      if ( (reference_keys_p != 0)
         && (reference_keys_p->nb_notified_keys > 0)
         && (reference_keys_p->nb_notified_keys <= KPD_NB_PHYSICAL_KEYS)
         && (new_keys_p != 0)
         && (new_keys_p->nb_notified_keys > 0)
         && (new_keys_p->nb_notified_keys <= KPD_NB_PHYSICAL_KEYS)
         && (reference_keys_p->nb_notified_keys == new_keys_p->nb_notified_keys) )
      {
         /* Check if all keys of reference_keys_p are defined in default mode */
         for (i = 0; i < reference_keys_p->nb_notified_keys; i++)
         {
            ret = kpd_retrieve_virtual_key_position(reference_keys_p->notified_keys[i],
                                                    KPD_DEFAULT_MODE,&position);
            if (ret == RV_INVALID_PARAMETER)
               return ret;
         }

         if (ret == RV_OK)
         {
            /* Reserve memory for message */
            mb_status = rvf_get_buf (kpd_env_ctrl_blk->prim_id, sizeof(T_KPD_SET_CONFIG_MODE_MSG), (void **) &msg_set_config_mode_p);

            if (mb_status != RVF_RED) /* Memory allocation success */
            {
               /* Fill the message */
               msg_set_config_mode_p->hdr.msg_id = KPD_SET_CONFIG_MODE_MSG;
               msg_set_config_mode_p->subscriber_id = subscriber_id;
               msg_set_config_mode_p->reference_keys.nb_notified_keys = reference_keys_p->nb_notified_keys;
               for (i = 0; i < reference_keys_p->nb_notified_keys; i++)
                  msg_set_config_mode_p->reference_keys.notified_keys[i] = reference_keys_p->notified_keys[i];
               msg_set_config_mode_p->new_keys.nb_notified_keys = new_keys_p->nb_notified_keys;
               for (i = 0; i < new_keys_p->nb_notified_keys; i++)
                  msg_set_config_mode_p->new_keys.notified_keys[i] = new_keys_p->notified_keys[i];

               /* Send message to the keypad task */
               rvf_send_msg(kpd_env_ctrl_blk->addr_id, msg_set_config_mode_p);
            }
            else
            {
               KPD_SEND_TRACE("KPD: Memory allocation error", RV_TRACE_LEVEL_ERROR);
               ret = RV_MEMORY_ERR;
            }
         }
      }
   }
   return ret;

#else
   return RV_NOT_SUPPORTED;
#endif
}

/**
 * function: kpd_get_available_keys
 */
T_RV_RET kpd_get_available_keys( T_KPD_VIRTUAL_KEY_TABLE* available_keys_p)
{
   kpd_get_default_keys(available_keys_p);
   return RV_OK;
}

/**
 * function: kpd_get_ascii_key_code
 */
T_RV_RET kpd_get_ascii_key_code( T_KPD_VIRTUAL_KEY_ID key,
                                 T_KPD_MODE mode,
                                 char** ascii_code_pp)
{
   INT8 position;

   /* Check if mode is authorized */ 
   if ( (mode !=  KPD_DEFAULT_MODE) && (mode !=  KPD_ALPHANUMERIC_MODE) )
      return RV_INVALID_PARAMETER;

   /* Check if key exist in the defined mode */ 
   kpd_retrieve_virtual_key_position(key, mode, &position);
   if (position == KPD_POS_NOT_AVAILABLE)
      return RV_INVALID_PARAMETER;

   /* Retrieve ASCII key value */
   kpd_get_ascii_key_value(position, mode, ascii_code_pp);
   return RV_OK;
}

/**
 * function: KP_Init
 */
void KP_Init( void(pressed(T_KPD_VIRTUAL_KEY_ID)), void(released(void)) )
{
   Kp.pressed = pressed;
   Kp.released = released;
}


#if ((CHIPSET == 12) || (CHIPSET == 15))

/**
 * function: kpd_register_for_timerchanges
 */
T_RV_RET kpd_register_for_timerchanges( T_KPD_SUBSCRIBER subscriber,
                                        T_KPD_TIMER_TYPE timer_type)
{
  T_RVF_MB_STATUS mb_status;
  T_KPD_REGISTER_FOR_TIMERCHANGES_MSG* msg_set_timer_p;
  T_RV_RET ret = RV_INVALID_PARAMETER;
  T_SUBSCRIBER_ID subscriber_id;

  /**
   * Check input parameters
   */

  /* Check subscriber id */
  if ( kpd_subscriber_id_used(subscriber, &subscriber_id) != TRUE)  
  {
    KPD_SEND_TRACE("KPD: Incorrect subscriber ID", RV_TRACE_LEVEL_ERROR);
    return ret;
  }
  /* Check timer type */
  if(timer_type > (KPD_TIMER_LONG_PRESS | KPD_TIMER_REPEAT)) 
  {
    KPD_SEND_TRACE("KPD: Invalid timer type", RV_TRACE_LEVEL_ERROR);
    return ret;
  }
  
  /**
   * Create and Send message
   */

  /* Reserve memory for message */
  mb_status = rvf_get_buf (kpd_env_ctrl_blk->prim_id, sizeof(T_KPD_REGISTER_FOR_TIMERCHANGES_MSG), (void **) &msg_set_timer_p);

  if (mb_status != RVF_RED) /* Memory allocation success */
  {
    /* Fill the message */
    msg_set_timer_p->hdr.msg_id = KPD_REGISTER_FOR_TIMERCHANGES_MSG;
    msg_set_timer_p->subscriber_id = subscriber_id;
    msg_set_timer_p->timer_type = timer_type;

    /* Send message to the keypad task */
    rvf_send_msg(kpd_env_ctrl_blk->addr_id, msg_set_timer_p);

    ret = RV_OK;
  }
  else
  {
    KPD_SEND_TRACE("KPD: Memory allocation error", RV_TRACE_LEVEL_ERROR);
    ret = RV_MEMORY_ERR;
  }
  
  /**
   * Return function results
   */
  return ret;
}


/**
 * function: kpd_register_for_keypad_timeout
 */
T_RV_RET kpd_register_for_keypad_timeout( T_KPD_SUBSCRIBER    subscriber,
                                          T_KPD_TIMEOUT_TYPE  timeout_type,
                                          UINT16 timeout_time)
{
  T_RVF_MB_STATUS mb_status;
  T_KPD_REGISTER_FOR_KEYPAD_TIMEOUT_MSG* msg_set_timeout_p;
  T_RV_RET ret = RV_INVALID_PARAMETER;
  T_SUBSCRIBER_ID subscriber_id;

  /**
   * Check input parameters
   */

  /* Check subscriber id */
  if ( kpd_subscriber_id_used(subscriber, &subscriber_id) != TRUE)  
  {
    KPD_SEND_TRACE("KPD: Incorrect subscriber ID", RV_TRACE_LEVEL_ERROR);
    return ret;
  }
  /* Check timeout type */
  if(timeout_type > (KPD_TIMEOUT_NO_KEY | KPD_TIMEOUT_KEY_JAM))
  {
    KPD_SEND_TRACE("KPD: Invalid timeout type", RV_TRACE_LEVEL_ERROR);
    return ret;
  }
  
  /**
   * Create and Send message
   */

  /* Reserve memory for message */
  mb_status = rvf_get_buf (kpd_env_ctrl_blk->prim_id, sizeof(T_KPD_REGISTER_FOR_KEYPAD_TIMEOUT_MSG), (void **) &msg_set_timeout_p);

  if (mb_status != RVF_RED) /* Memory allocation success */
  {
    /* Fill the message */
    msg_set_timeout_p->hdr.msg_id = KPD_REGISTER_FOR_KEYPAD_TIMEOUT_MSG;
    msg_set_timeout_p->subscriber_id = subscriber_id;
    msg_set_timeout_p->timeout_type = timeout_type;
    msg_set_timeout_p->timeout_time = timeout_time;

    /* Send message to the keypad task */
    rvf_send_msg(kpd_env_ctrl_blk->addr_id, msg_set_timeout_p);

    ret = RV_OK;
  }
  else
  {
    KPD_SEND_TRACE("KPD: Memory allocation error", RV_TRACE_LEVEL_ERROR);
    ret = RV_MEMORY_ERR;
  }
  
  /**
   * Return function results
   */
  return ret;
}

/**
 * function: kpd_register_for_miss_event
 */
T_RV_RET kpd_register_for_miss_event( T_KPD_SUBSCRIBER subscriber,
                                      T_KPD_MISS_EVENT_TYPE missevent_type)
{
  T_RVF_MB_STATUS mb_status;
  T_KPD_REGISTER_FOR_MISS_EVENT_MSG* msg_set_miss_event_p;
  T_RV_RET ret = RV_INVALID_PARAMETER;
  T_SUBSCRIBER_ID subscriber_id;

  /**
   * Check input parameters
   */

  /* Check subscriber id */
  if ( kpd_subscriber_id_used(subscriber, &subscriber_id) != TRUE)  
  {
    KPD_SEND_TRACE("KPD: Incorrect subscriber ID", RV_TRACE_LEVEL_ERROR);
    return ret;
  }
  /* Check miss event type */
  if(missevent_type > KPD_MISS_EVENT_ON)
  {
    KPD_SEND_TRACE("KPD: Invalid miss event type", RV_TRACE_LEVEL_ERROR);
    return ret;
  }
  
  /**
   * Create and Send message
   */

  /* Reserve memory for message */
  mb_status = rvf_get_buf (kpd_env_ctrl_blk->prim_id, sizeof(T_KPD_REGISTER_FOR_MISS_EVENT_MSG), (void **) &msg_set_miss_event_p);

  if (mb_status != RVF_RED) /* Memory allocation success */
  {
    /* Fill the message */
    msg_set_miss_event_p->hdr.msg_id = KPD_REGISTER_FOR_MISS_EVENT_MSG;
    msg_set_miss_event_p->subscriber_id = subscriber_id;
    msg_set_miss_event_p->missevent_type = missevent_type;

    /* Send message to the keypad task */
    rvf_send_msg(kpd_env_ctrl_blk->addr_id, msg_set_miss_event_p);

    ret = RV_OK;
  }
  else
  {
    KPD_SEND_TRACE("KPD: Memory allocation error", RV_TRACE_LEVEL_ERROR);
    ret = RV_MEMORY_ERR;
  }
  
  /**
   * Return function results
   */
  return ret;
}

#endif /* (CHIPSET == 12) */


/*@}*/
