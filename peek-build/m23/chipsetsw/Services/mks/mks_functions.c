/**
 * @file   mks_functions.c
 *
 * Implementation of MKS functions.
 *
 * @author   Laurent Sollier (l-sollier@ti.com)
 * @version 0.1
 */

/*
 * History:
 *
 *   Date          Author       Modification
 *  ----------------------------------------
 *  11/16/2001     L Sollier    Create
 *
 *
 * (C) Copyright 2001 by Texas Instruments Incorporated, All Rights Reserved
 */

#include "mks/mks_i.h"
#include "mks/mks_api.h"
#include "mks/mks_env.h"

#include "kpd/kpd_api.h"

#include "rvm/rvm_use_id_list.h"
#include "rv/rv_general.h"

#include <string.h>


/* This value is set in the T_KEY_SEQUENCE_INFOS structure when post-sequence is on going */
#define POST_SEQUENCE_COMPLETED 0x10

/** This structure defines  */
typedef struct {  char name[KPD_MAX_CHAR_NAME+1];
                  T_KPD_VIRTUAL_KEY_ID sequence_key[MKS_NB_MAX_OF_KEY_IN_KEY_SEQUENCE];
                  UINT8 nb_key_of_sequence;
                  UINT8 completion_type;
                  UINT8 nb_key_for_post_sequence;
                  UINT8 nb_key_sent_for_post_sequence;
                  T_RV_RETURN return_path;
               } T_KEY_SEQUENCE_INFOS;




/** Definition of the different magic key sequence.
 * Note that the magic key sequence is defined by a KPD_KEY_NULL key Id.
 */
static T_KEY_SEQUENCE_INFOS* key_sequence_table[MKS_NB_MAX_OF_KEY_SEQUENCE] = {0};

/* Subscriber Id for keypad driver */
static T_KPD_SUBSCRIBER subscriber_id;

/* Number of key sequence registered */
static UINT8 nb_key_sequence = 0;

/* Key pressed history */
static T_KPD_VIRTUAL_KEY_ID key_history[MKS_NB_MAX_OF_KEY_IN_KEY_SEQUENCE];


/** External declaration */
extern T_MKS_ENV_CTRL_BLK* mks_env_ctrl_blk;

/**
 * @name Functions implementation
 *
 */
/*@{*/


/**
 * function: mks_add_key_sequence_i
 */
T_RV_RET mks_add_key_sequence_i(T_MKS_INFOS_KEY_SEQUENCE_MSG* infos_key_sequence_p)
{
   UINT8 i;
   T_RVF_MB_STATUS mb_status;
   T_RV_RET ret = RV_OK;
   T_MKS_INFOS_KEY_SEQUENCE* infos_p = &(infos_key_sequence_p->key_sequence_infos);

   if (nb_key_sequence < MKS_NB_MAX_OF_KEY_SEQUENCE)
   {
      /* Reserve memory for message */
      mb_status = rvf_get_buf (mks_env_ctrl_blk->prim_id, sizeof(T_KEY_SEQUENCE_INFOS), (void **) &key_sequence_table[nb_key_sequence]);   

      if (mb_status != RVF_RED) /* Memory allocation success */
      {
         /* Fill the structure */
         for (i = 0; i < infos_p->nb_key_of_sequence; i++)
            key_sequence_table[nb_key_sequence]->sequence_key[i] = infos_p->key_id[i];

         key_sequence_table[nb_key_sequence]->nb_key_of_sequence = infos_p->nb_key_of_sequence;
         key_sequence_table[nb_key_sequence]->completion_type = infos_p->completion_type;
         key_sequence_table[nb_key_sequence]->return_path = infos_p->return_path;
         key_sequence_table[nb_key_sequence]->nb_key_for_post_sequence = infos_p->nb_key_for_post_sequence;
         infos_p->name[KPD_MAX_CHAR_NAME] = 0;
         strcpy(key_sequence_table[nb_key_sequence]->name, infos_p->name);

         key_sequence_table[nb_key_sequence]->nb_key_sent_for_post_sequence = 0;

         /* Update number of key sequence */
         nb_key_sequence++;

         ret = RV_OK;
      }
      else
      {
         MKS_SEND_TRACE("MKS: Memory allocation error", RV_TRACE_LEVEL_ERROR);
         ret = RV_MEMORY_ERR;
      }
   }
   else
   {
      /* Maximum of magic key sequence registered*/
      ret = RV_INTERNAL_ERR;
   }

   return ret;
}

/**
 * function: mks_remove_key_sequence_i
 *
 */
T_RV_RET mks_remove_key_sequence_i(T_MKS_REMOVE_KEY_SEQUENCE_MSG* remove_key_sequence_p)
{
   UINT8 i;

   for (i = 0; i < nb_key_sequence; i++)
   {
      /* Compare name of the key sequence */
      if ( !(strcmp(key_sequence_table[i]->name, remove_key_sequence_p->name)) )
      {
         /* Free memory used for the key sequence */
         rvf_free_buf(key_sequence_table[i]);
         nb_key_sequence--;

         /* Sort out the table of key sequence */
         key_sequence_table[i] = key_sequence_table[nb_key_sequence];
         key_sequence_table[nb_key_sequence] = 0;
         break;
      }
   }

   return RV_OK;
}

/**
 * function: mks_check_key_sequence
 */
void mks_check_key_sequence(T_KPD_KEY_EVENT_MSG* key_event_p)
{
   UINT8 i,j, nb_key_of_sequence;
   BOOL key_sequence_completed = TRUE;

   /* Stop timer */
   rvf_stop_timer(RVF_TIMER_0);

   /* Update key pressed history */
   for (i = 0; i < MKS_NB_MAX_OF_KEY_IN_KEY_SEQUENCE-1; i++)
      key_history[i] = key_history[i+1];

   key_history[MKS_NB_MAX_OF_KEY_IN_KEY_SEQUENCE-1] = key_event_p->key_info.virtual_key_id;

   /* Check each key sequence */
   for (i = 0; i < nb_key_sequence; i++)
   {
      if (!(key_sequence_table[i]->completion_type & POST_SEQUENCE_COMPLETED))
      {
         /* Verify if magic key sequence is completed */
         nb_key_of_sequence = key_sequence_table[i]->nb_key_of_sequence;
         for (j = 0; j < nb_key_of_sequence; j++)
         {
            if (key_sequence_table[i]->sequence_key[nb_key_of_sequence-j-1] != key_history[MKS_NB_MAX_OF_KEY_IN_KEY_SEQUENCE-j-1])
            {
               key_sequence_completed = FALSE;
               break;
            }
         }
         if (key_sequence_completed == TRUE)
         {
            if (key_sequence_table[i]->completion_type == MKS_SEQUENCE_COMPLETED)
            {
               /* Process sequence completed */
               mks_send_key_sequence_completed_msg(MKS_SEQUENCE_COMPLETED,
                                                   KPD_KEY_NULL,
                                                   key_sequence_table[i]->return_path,
                                                   key_sequence_table[i]->name);
            }
            else
            {
               /* Store info that post-sequence is on going */
               key_sequence_table[i]->completion_type |= POST_SEQUENCE_COMPLETED;
            }
         }
         else
            key_sequence_completed = TRUE;
      }
      else
      {
         /* Notify that magic key sequence is completed */
         mks_send_key_sequence_completed_msg(MKS_POST_SEQUENCE,
                                             key_event_p->key_info.virtual_key_id,
                                             key_sequence_table[i]->return_path,
                                             key_sequence_table[i]->name);

         key_sequence_table[i]->nb_key_sent_for_post_sequence++;

         if (key_sequence_table[i]->nb_key_sent_for_post_sequence == key_sequence_table[i]->nb_key_for_post_sequence)
         {
            key_sequence_table[i]->nb_key_sent_for_post_sequence = 0;
            key_sequence_table[i]->completion_type &= ~POST_SEQUENCE_COMPLETED;
         }
      }
   }

   rvf_start_timer(RVF_TIMER_0, RVF_MS_TO_TICKS(MKS_TIME_RESET_KEY_SEQUENCE), FALSE);
}



/**
 * function: mks_send_key_sequence_completed_msg
 */
void mks_send_key_sequence_completed_msg(UINT8 completion_level,
                                         T_KPD_VIRTUAL_KEY_ID key_id,
                                         T_RV_RETURN return_path,
                                         char* name)
{
   T_MKS_SEQUENCE_COMPLETED_MSG* msg_sequence_completed = 0;
   T_RVF_MB_STATUS mb_status;

   /* Reserve memory for message */
   mb_status = rvf_get_buf (mks_env_ctrl_blk->prim_id, sizeof(T_MKS_SEQUENCE_COMPLETED_MSG), (void **) &msg_sequence_completed);   

   if (mb_status != RVF_RED) /* Memory allocation success */
   {
      /* Fill the message */
      msg_sequence_completed->hdr.msg_id = MKS_SEQUENCE_COMPLETED_MSG;
      strcpy(msg_sequence_completed->name, name);
      msg_sequence_completed->completion_type = completion_level;
      msg_sequence_completed->key_pressed = key_id;

      /* Send message to the client */
      if (return_path.callback_func != 0)
      {
         return_path.callback_func((void*) msg_sequence_completed);
         rvf_free_buf(msg_sequence_completed);
      }
      else
      {
         rvf_send_msg(return_path.addr_id, msg_sequence_completed);
      }
   }
   else
   {
      MKS_SEND_TRACE("MKS: Memory allocation error", RV_TRACE_LEVEL_ERROR);
   }
}


/**
 * function: mks_reset_sequence
 */
void mks_reset_sequence(void)
{
   UINT8 i;

   for (i = 0; i < MKS_NB_MAX_OF_KEY_IN_KEY_SEQUENCE; i++)
      key_history[i] = KPD_KEY_NULL;
}


/**
 * function: mks_initialize_swe
 */
void mks_initialize_swe(void)
{
   T_RV_RET ret;
   T_KPD_VIRTUAL_KEY_TABLE notified_keys;
   T_RV_RETURN return_path;
   UINT8 i;

   /* Initialize key pressed history structure */
   for (i = 0; i < MKS_NB_MAX_OF_KEY_IN_KEY_SEQUENCE; i++)
      key_history[i] = KPD_KEY_NULL;

   /* Subscription to the keypad */
   return_path.addr_id = mks_env_ctrl_blk->addr_id;
   return_path.callback_func = 0;
   notified_keys.nb_notified_keys = KPD_NB_PHYSICAL_KEYS;
   ret = kpd_subscribe (&subscriber_id, KPD_DEFAULT_MODE, &notified_keys, return_path);

   if (ret != RV_OK)
   {
      MKS_SEND_TRACE("MKS: Unable to subscribe to the keypad", RV_TRACE_LEVEL_DEBUG_HIGH);
   }
}


/**
 * function: mks_stop_swe
 */
void mks_stop_swe(void)
{
   T_RV_RET ret;

   /* Unsubscribe to the keypad */
   ret = kpd_unsubscribe(&subscriber_id);

   if (ret != RV_OK)
   {
      MKS_SEND_TRACE("MKS: Unsubscription error", RV_TRACE_LEVEL_DEBUG_HIGH);
   }
}


/*@}*/
