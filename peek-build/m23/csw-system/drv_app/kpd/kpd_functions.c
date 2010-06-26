/**
 * @file   kpd_functions.c
 *
 * Implementation of Keypad functions.
 * These functions implement the keypad processing.
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

#include "kpd/kpd_i.h"
#include "kpd/kpd_virtual_key_table_mgt.h"
#include "kpd/kpd_env.h"
#include "kpd/kpd_messages_i.h"

#include "kpd/kpd_scan_functions.h"
#include "kpd/kpd_process_internal_msg.h"

#include "rvf/rvf_api.h"
#include "rvm/rvm_use_id_list.h"

extern T_KPD_ENV_CTRL_BLK* kpd_env_ctrl_blk;

#define DEBUG_ARRAY_INDEX 128
/* Define the max of subscribers supported by the keypad driver */
#define MAX_SUBSCRIBERS 32


#define KEY_MASK_SIZE_FOR_SN_MODE 20

/*#define IS_VALID_BIT(pos)  ( (pos>=0 && pos<=4) || (pos>=8 && pos<=12)) : look for valid bits in KPD full code regs */
#define IS_VALID_BIT(pos)  ( (pos<=4) || (pos>=8 && pos<=12)) /*look for valid bits in KPD full code regs */

/* This structure gather general informations about keypad */
typedef struct {  UINT32               reserved_subscriber_id;
                  UINT8                keypad_mode;
                  char                 sn_mode_notified_keys[KEY_MASK_SIZE_FOR_SN_MODE];
                  T_SUBSCRIBER_ID      owner_keypad_id;
               } T_KEYPAD_DRIVER_INFOS;

/* Keypad informations */
static T_KEYPAD_DRIVER_INFOS keypad_driver_infos;

/* Mutex used to protect reserved_subscriber_id variable */
static T_RVF_MUTEX mutex;

UINT16 kpd_configured_long_key_time;
UINT16 kpd_configured_repeat_key_time;

/**
 * @name Functions implementation
 *
 */
/*@{*/


/**
 * function: kpd_initialize_keypad_driver
 */
T_RV_RET kpd_initialize_keypad_driver(void)
{
   T_RV_RET ret = RV_OK;
   UINT8 i;

   /* Initialization of keypad_driver_infos */
   keypad_driver_infos.reserved_subscriber_id = 0;
   keypad_driver_infos.keypad_mode = MN_MODE;
   for (i = 0; i < KEY_MASK_SIZE_FOR_SN_MODE; i++)
      keypad_driver_infos.sn_mode_notified_keys[i] = 0;
   keypad_driver_infos.owner_keypad_id = 0;

#if ((CHIPSET == 12) || (CHIPSET == 15))
   kpd_clr_subscriber_table();

   /* Set default timer values for long time and repeat time */
   kpd_env_ctrl_blk->long_time = KPD_DEFAULT_LONG_TIME;
   kpd_env_ctrl_blk->repeat_time = KPD_DEFAULT_REPEAT_TIME;
   kpd_configured_long_key_time=KPD_DEFAULT_LONG_TIME;
   kpd_configured_repeat_key_time = KPD_DEFAULT_REPEAT_TIME;
   /* Initialization of received key info */
   for(i=0; i<KPD_MAX_DETECTABLE; i++)
   {
      kpd_env_ctrl_blk->received_key_info[i].key = KPD_PKEY_NULL;
      kpd_env_ctrl_blk->received_key_info[i].key_state = released;
   }
   kpd_env_ctrl_blk->nb_active_keys = 0;
#endif



   /* Initialize ASCII table */
   if (kpd_initialize_ascii_table() != RV_OK)
      ret = RV_INTERNAL_ERR;

   /* Check if number max of subscriber is supported by the driver */
   else if (KPD_MAX_SUBSCRIBER > MAX_SUBSCRIBERS)
      ret = RV_INTERNAL_ERR;

   /* Check validity of the vpm table */
   else if (kpd_vpm_table_is_valid() == FALSE)
      ret = RV_INTERNAL_ERR;

   /* Mutex initialization */
//   else if (rvf_initialize_static_mutex(&mutex) != RVF_OK)
   else if (rvf_initialize_static_mutex(&mutex) != RVF_OK)
      ret =  RV_INTERNAL_ERR;

   /* Hardware initialization */
   kpd_initialize_keypad_hardware();

   if (ret ==  RV_INTERNAL_ERR)
      KPD_SEND_TRACE("Keypad driver initialization failed", RV_TRACE_LEVEL_ERROR);

   return ret;
}

/**
 * function: kpd_kill_keypad_driver
 */
T_RV_RET kpd_kill_keypad_driver(void)
{
   return rvf_delete_mutex(&mutex);
}


/**
 * function: kpd_add_subscriber
 */
T_RV_RET kpd_add_subscriber(T_SUBSCRIBER_ID* subscriber_id)
{
   UINT8 i;
   UINT8 nb_subscriber = 0;
   T_RV_RET ret = RV_OK;

   rvf_lock_mutex(&mutex);

   /* Check number of subscribers */
   for (i = 0; i < KPD_MAX_SUBSCRIBER; i++)
      if (keypad_driver_infos.reserved_subscriber_id & (1<<i))
         nb_subscriber++;

   if (nb_subscriber >= KPD_MAX_SUBSCRIBER)
   {
      KPD_SEND_TRACE("KPD: Max of subscriber reached", RV_TRACE_LEVEL_WARNING);
      ret = RV_INTERNAL_ERR;
   }
   else
   {
      for (i = 0; i < KPD_MAX_SUBSCRIBER; i++)
         if ( (keypad_driver_infos.reserved_subscriber_id & (1<<i)) == 0)
         {
            keypad_driver_infos.reserved_subscriber_id |= 1<<i;
            *subscriber_id = i;
            break;
         }
   }

   rvf_unlock_mutex(&mutex);

   return ret;
}

/**
 * function: kpd_remove_subscriber
 */
T_RV_RET kpd_remove_subscriber(T_SUBSCRIBER_ID subscriber_id)
{
   T_RV_RET ret = RV_OK;

   rvf_lock_mutex(&mutex);

   /* Check if subscriber id is correct */
   if (keypad_driver_infos.reserved_subscriber_id & (1<<subscriber_id) )
   {
      /* Unreserve the id */
      keypad_driver_infos.reserved_subscriber_id &= ~(1<<subscriber_id);
   }
   else
   {
      KPD_SEND_TRACE("KPD: Subscriber Id unknown", RV_TRACE_LEVEL_ERROR);
      ret = RV_INVALID_PARAMETER;
   }

   rvf_unlock_mutex(&mutex);

   return ret;
}

/**
 * function: kpd_subscriber_id_used
 */
BOOL kpd_subscriber_id_used(T_KPD_SUBSCRIBER subscriber, T_SUBSCRIBER_ID* subscriber_id)
{
   BOOL ret = FALSE;
   T_SUBSCRIBER_ID id;

   rvf_lock_mutex(&mutex);

   if (subscriber != 0)
   {
      id = ((T_SUBSCRIBER*) subscriber)->subscriber_id;
      *subscriber_id = id;

      /* Check if subscriber id is correct */
      if ( keypad_driver_infos.reserved_subscriber_id & (1<<id) )
         ret = TRUE;
   }

   rvf_unlock_mutex(&mutex);

   return ret;
}


/**
 * function: kpd_send_key_event_message
 */
void kpd_send_key_event_message(T_KPD_PHYSICAL_KEY_ID physical_key_pressed_id,
                                T_KPD_KEY_STATE state,
                                T_KPD_PRESS_STATE press_state,
                                T_KPD_MODE mode,
                                T_RV_RETURN return_path)
{
   T_RVF_MB_STATUS mb_status;
   T_KPD_KEY_EVENT_MSG* key_event;
   char* ascii_code;


   /* Subscriber must be notified by the pressed key */
   mb_status = rvf_get_buf (kpd_env_ctrl_blk->prim_id, sizeof(T_KPD_KEY_EVENT_MSG), (void **) &key_event);   

   if (mb_status != RVF_RED) /* Memory allocation success */
   {
      /* Fill the message */
      key_event->hdr.msg_id = KPD_KEY_EVENT_MSG;
      key_event->key_info.virtual_key_id = kpd_get_virtual_key(physical_key_pressed_id,
                                                               mode);
      key_event->key_info.state = state;
      key_event->key_info.press_state = press_state;
      kpd_get_ascii_key_value(physical_key_pressed_id,
                              mode,
                              &ascii_code);
      key_event->key_info.ascii_value_p = ascii_code;
      
      KPD_SEND_TRACE_PARAM("KPD: Virtual key Id sent: ", key_event->key_info.virtual_key_id, RV_TRACE_LEVEL_DEBUG_HIGH);

      /* Send message to the client */
      if (return_path.callback_func != 0)
      {
         return_path.callback_func((void*) key_event);
         rvf_free_buf(key_event);
      }
      else
      {
         rvf_send_msg(return_path.addr_id, key_event);
      }
   }
   else
   {
      KPD_SEND_TRACE("KPD: Memory allocation error", RV_TRACE_LEVEL_ERROR);
   }
}


/**
 * function: kpd_send_status_message
 */
void kpd_send_status_message(UINT8 operation, UINT8 status_value, T_RV_RETURN return_path)
{
   T_RVF_MB_STATUS mb_status;
   T_KPD_STATUS_MSG* msg_status;

   /* Reserve memory for message */
   mb_status = rvf_get_buf (kpd_env_ctrl_blk->prim_id, sizeof(T_KPD_STATUS_MSG), (void **) &msg_status);

   if (mb_status != RVF_RED) /* Memory allocation success */
   {
      /* Fill the message */
      msg_status->hdr.msg_id = KPD_STATUS_MSG;
      msg_status->operation = operation;
      msg_status->status_value = status_value;

      /* Send message to the client */
      if (return_path.callback_func != 0)
      {
         return_path.callback_func((void*) msg_status);
         rvf_free_buf(msg_status);

      }
      else
      {
         rvf_send_msg(return_path.addr_id, msg_status);
      }
      KPD_SEND_TRACE_PARAM("KPD: Sent status message, Id:", status_value, RV_TRACE_LEVEL_DEBUG_LOW);
   }
   else
   {
      KPD_SEND_TRACE("KPD: Memory allocation error", RV_TRACE_LEVEL_ERROR);
   }
}


/**
 * function: kpd_is_key_in_sn_mode
 */
BOOL kpd_is_key_in_sn_mode(T_KPD_PHYSICAL_KEY_ID physical_key_pressed_id)
{
   return ( (keypad_driver_infos.keypad_mode == SN_MODE)
          && (keypad_driver_infos.sn_mode_notified_keys[physical_key_pressed_id >> 3] & (1<<(physical_key_pressed_id & 0x07))) );
}


/**
 * function: kpd_set_keys_in_sn_mode
 */
void kpd_set_keys_in_sn_mode(T_KPD_VIRTUAL_KEY_TABLE* keys_owner, T_KPD_MODE mode)
{
   UINT8 i;
   INT8 position;

   for (i = 0; i < keys_owner->nb_notified_keys; i++)
   {
      /* Retrieve position in vpm table */
      kpd_retrieve_virtual_key_position(keys_owner->notified_keys[i],
                                        mode,
                                        &position);

      keypad_driver_infos.sn_mode_notified_keys[position >> 3] |= (1<<(position & 0x07));
   }
}


/**
 * function: kpd_is_owner_keypad
 */
BOOL kpd_is_owner_keypad(T_SUBSCRIBER_ID subscriber_id)
{
   return ( (keypad_driver_infos.keypad_mode == SN_MODE)
          && (keypad_driver_infos.owner_keypad_id == subscriber_id) );
}


/**
 * function: kpd_get_keypad_mode
 */
UINT8 kpd_get_keypad_mode(void)
{
   return keypad_driver_infos.keypad_mode;
}

/**
 * function: kpd_set_keypad_mode
 */
void kpd_set_keypad_mode(UINT8 mode)
{
   UINT8 i;

   keypad_driver_infos.keypad_mode = mode;

   if (mode == MN_MODE)
   {
      for (i = 0; i < KEY_MASK_SIZE_FOR_SN_MODE; i++)
         keypad_driver_infos.sn_mode_notified_keys[i] = 0;
   }
}

/**
 * function: kpd_get_owner_keypad_id
 */
T_SUBSCRIBER_ID kpd_get_owner_keypad_id(void)
{
   return keypad_driver_infos.owner_keypad_id;
}

/**
 * function: kpd_set_owner_keypad_id
 */
void kpd_set_owner_keypad_id(T_SUBSCRIBER_ID subscriber_id)
{
   keypad_driver_infos.owner_keypad_id = subscriber_id;
}


#if ((CHIPSET == 12) || (CHIPSET == 15))

/**
 * function : kpd_decode_key_registers
 */
UINT8 kpd_decode_key_registers(T_RV_HDR* msg_p, INT8* received_keys_p)
{
  UINT8     nb_keys_decoded=0;
  UINT8     i;
  UINT16    key_reg[4];
  UINT8     count=0;
  BOOLEAN   done = FALSE;
  T_KPD_KEY_EVENT_INTERRUPT_MSG* msg_key_event_p= (T_KPD_KEY_EVENT_INTERRUPT_MSG*) msg_p;


  key_reg[0] = msg_key_event_p->full_code_15_0;
  key_reg[1] = msg_key_event_p->full_code_31_16;
  key_reg[2] = msg_key_event_p->full_code_47_32;
  key_reg[3] = msg_key_event_p->full_code_63_48;

  for(i=0; i<3; i++)
  {
    while(!done && (nb_keys_decoded<KPD_MAX_DETECTABLE))
    {
      /* Are there bits set in the register ? */
      if (key_reg[i] > 0)
      {
        if((key_reg[i] & 0x0001) && IS_VALID_BIT(count))
        {
          /* save key value */
          received_keys_p[nb_keys_decoded] = kpd_map_reg_on_layout(count + (i * 16));
          nb_keys_decoded++;
          count++;
        }
        else
        {
          count++;
        }
        /* shift 1 right */
        key_reg[i] >>= 1;
      }
      else
      {
        /* No more bits set in this register */
        done = TRUE;
      }
    }
    /* Reset values */
    count = 0;
    done = FALSE;
  }

  /* Return the number of keys decoded */
  return nb_keys_decoded;
}

/**
 * function : kpd_update_received_key_info
 */
void kpd_update_received_key_info(UINT8 nb_keys_pressed, INT8* key_pressed_id)
{
  T_KPD_RECEIVED_KEY_INFO received_key_info_tmp[KPD_MAX_DETECTABLE];
  UINT8                   i,j;
  UINT8                   nb_active_keys=0;
  BOOLEAN                 found = FALSE;

  /* Clear received_key_info_tmp */
  for(i=0; i<KPD_MAX_DETECTABLE; i++)
  {
    received_key_info_tmp[i].key = KPD_PKEY_NULL;
    received_key_info_tmp[i].key_state = released;
  }


  /* remove "released" keys from table */
  for(i=0; i<kpd_env_ctrl_blk->nb_active_keys; i++)
  {
    /* Save the keys that do not have the status "released" */
    if(( kpd_env_ctrl_blk->received_key_info[i].key_state != released) &&
       ( kpd_env_ctrl_blk->received_key_info[i].key != KPD_PKEY_NULL))
    {
      received_key_info_tmp[nb_active_keys].key_state = kpd_env_ctrl_blk->received_key_info[i].key_state;
      received_key_info_tmp[nb_active_keys].key = kpd_env_ctrl_blk->received_key_info[i].key;
      nb_active_keys++;
    }
  }
  /* remove active keys that are released */
  for(i=0; i<nb_active_keys; i++)
  {
    found = FALSE;
    for(j=0; j<nb_keys_pressed; j++)
    {
      if(received_key_info_tmp[i].key == key_pressed_id[j])
      {
        found = TRUE;
        break;
      }
    }
    if(found == FALSE)
    {
      received_key_info_tmp[i].key_state = released;
    }
  }
  /* update status of existings keys */
  for(i=0; i<nb_active_keys; i++)
  {
    for(j=0; j<nb_keys_pressed; j++)
    {
      if(received_key_info_tmp[i].key == key_pressed_id[j])
      {
        if(received_key_info_tmp[i].key_state == first_press)
        {
          received_key_info_tmp[i].key_state = key_press;
        }
        key_pressed_id[j] = KPD_PKEY_NULL;
        break;
      }
    }
  }
  /* add new keys to the table */
  for(i=0; i<KPD_MAX_DETECTABLE; i++)
  {
    /* New key == value > -1 */
    if(key_pressed_id[i] > KPD_PKEY_NULL) 
    {
      received_key_info_tmp[nb_active_keys].key       = key_pressed_id[i];
      received_key_info_tmp[nb_active_keys].key_state = first_press;
      nb_active_keys++;
    }
  }


  /* Copy temporary information back main data table */
  for(i=0; i<KPD_MAX_DETECTABLE; i++)
  {
    kpd_env_ctrl_blk->received_key_info[i].key = received_key_info_tmp[i].key;
    kpd_env_ctrl_blk->received_key_info[i].key_state = received_key_info_tmp[i].key_state;
  }

    
/*OMAPS00109838*/
#if 1
  if(nb_active_keys > KPD_MAX_DETECTABLE)
  {
  	  kpd_env_ctrl_blk->nb_active_keys = KPD_MAX_DETECTABLE;
  }
  else
  {
        kpd_env_ctrl_blk->nb_active_keys = nb_active_keys;
  }
  #endif
  /*!OMAPS00109838*/
}



/**
 * function kpd_remove_released_from_key_info
 */
void kpd_remove_released_from_key_info(void)
{
  T_KPD_RECEIVED_KEY_INFO received_key_info_tmp[KPD_MAX_DETECTABLE];
  UINT8                   i;
  UINT8                   nb_active_keys=0;


  /* Clear received_key_info_tmp */
  for(i=0; i<KPD_MAX_DETECTABLE; i++)
  {
    received_key_info_tmp[i].key = KPD_PKEY_NULL;
    received_key_info_tmp[i].key_state = released;
  }



  /* remove "released" keys from table */
  for(i=0; (i<kpd_env_ctrl_blk->nb_active_keys && nb_active_keys<KPD_MAX_DETECTABLE); i++)
  {
    /* Save the keys that do not have the status "released" */
    if(( kpd_env_ctrl_blk->received_key_info[i].key_state != released) &&
       ( kpd_env_ctrl_blk->received_key_info[i].key != KPD_PKEY_NULL))
    {
      received_key_info_tmp[nb_active_keys].key_state = kpd_env_ctrl_blk->received_key_info[i].key_state;
      received_key_info_tmp[nb_active_keys].key = kpd_env_ctrl_blk->received_key_info[i].key;
      nb_active_keys++;
    }
  }
  
  /* Copy temporary information back main data table */
  for(i=0; i<KPD_MAX_DETECTABLE; i++)
  {
    kpd_env_ctrl_blk->received_key_info[i].key = received_key_info_tmp[i].key;
    kpd_env_ctrl_blk->received_key_info[i].key_state = received_key_info_tmp[i].key_state;
  }
  /*OMAPS00109838*/
  #if 1
  if(nb_active_keys > KPD_MAX_DETECTABLE)
  {
  	  kpd_env_ctrl_blk->nb_active_keys = KPD_MAX_DETECTABLE;
  }
  else
  {
        kpd_env_ctrl_blk->nb_active_keys = nb_active_keys;
  }
#endif
/*!OMAPS00109838*/
}


/**
 * function kpd_process_long_key_received_key_info
 */
void kpd_process_long_key_received_key_info(UINT8 nb_keys_pressed, INT8* key_pressed_id)
{
  UINT8 i,j;

  for(i=0; i<nb_keys_pressed; i++)
  {
    j=0; 

    while (key_pressed_id[i] != kpd_env_ctrl_blk->received_key_info[j].key)
    {
      j++;
    }
    if((kpd_env_ctrl_blk->received_key_info[j].key_state == first_press) ||
       (kpd_env_ctrl_blk->received_key_info[j].key_state == key_press))
    {
      kpd_env_ctrl_blk->received_key_info[j].key_state = long_press;
    }
    else if (kpd_env_ctrl_blk->received_key_info[j].key_state == long_press)
    {
      kpd_env_ctrl_blk->received_key_info[j].key_state = repeat_press;
    }
  }
}

#endif /* (CHIPSET==12) */
/*@}*/
