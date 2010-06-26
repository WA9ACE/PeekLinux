/**
 * @file   mks_api.c
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
 *  11/19/2001     L Sollier    Create
 *
 *
 * (C) Copyright 2001 by Texas Instruments Incorporated, All Rights Reserved
 */

#include "mks/mks_api.h"
#include "mks/mks_env.h"
#include "mks/mks_i.h"
#include "mks/mks_messages_i.h"

#include "rvm/rvm_use_id_list.h"

#include <string.h>

/** External declaration */
extern T_MKS_ENV_CTRL_BLK* mks_env_ctrl_blk;



/**
 * @name Bridge functions implementation
 *
 */
/*@{*/


/**
 * function: mks_add_key_sequence
 */
T_RV_RET mks_add_key_sequence(T_MKS_INFOS_KEY_SEQUENCE* infos_key_sequence_p)
{
   T_RV_RET ret = RV_INVALID_PARAMETER;
   T_MKS_INFOS_KEY_SEQUENCE_MSG* msg_p;
   T_RVF_MB_STATUS mb_status;

   /* Test validity of structure */
   if ((infos_key_sequence_p->nb_key_of_sequence >=3) && (infos_key_sequence_p->nb_key_of_sequence <= MKS_NB_MAX_OF_KEY_IN_KEY_SEQUENCE))
   {
      if ( (infos_key_sequence_p->completion_type == MKS_SEQUENCE_COMPLETED)
         || ( (infos_key_sequence_p->completion_type == MKS_POST_SEQUENCE)
             && (infos_key_sequence_p->nb_key_for_post_sequence > 0)
             && (infos_key_sequence_p->nb_key_for_post_sequence <= MKS_NB_MAX_OF_KEY_FOR_POST_SEQUENCE)) )
      {
         /* Reserve memory for message */
         mb_status = rvf_get_buf (mks_env_ctrl_blk->prim_id, sizeof(T_MKS_INFOS_KEY_SEQUENCE_MSG), (void **) &msg_p);   

         if (mb_status != RVF_RED) /* Memory allocation success */
         {
            /* Fill the message */
            msg_p->hdr.msg_id = MKS_INFOS_KEY_SEQUENCE_MSG;

            msg_p->key_sequence_infos = *infos_key_sequence_p;

            /* Send message to the MKS task */
            rvf_send_msg(mks_env_ctrl_blk->addr_id, msg_p);

            ret = RV_OK;
         }
         else
         {
            MKS_SEND_TRACE("MKS: Memory allocation error", RV_TRACE_LEVEL_ERROR);
            ret = RV_MEMORY_ERR;
         }
      }
   }

   return ret;
}

/**
 * function: mks_remove_key_sequence
 *
 */
T_RV_RET mks_remove_key_sequence(char name[KPD_MAX_CHAR_NAME+1])
{
   T_MKS_REMOVE_KEY_SEQUENCE_MSG* msg_p;
   T_RVF_MB_STATUS mb_status;
   T_RV_RET ret = RV_OK;

   /* Reserve memory for message */
   mb_status = rvf_get_buf (mks_env_ctrl_blk->prim_id, sizeof(T_MKS_REMOVE_KEY_SEQUENCE_MSG), (void **) &msg_p);   

   if (mb_status != RVF_RED) /* Memory allocation success */
   {
      /* Fill the message */
      msg_p->hdr.msg_id = MKS_REMOVE_KEY_SEQUENCE_MSG;

      memcpy(msg_p->name, name, KPD_MAX_CHAR_NAME+1);

      /* Send message to the MKS task */
      rvf_send_msg(mks_env_ctrl_blk->addr_id, msg_p);

      ret = RV_OK;
   }
   else
   {
      MKS_SEND_TRACE("MKS: Memory allocation error", RV_TRACE_LEVEL_ERROR);
      ret = RV_MEMORY_ERR;
   }


   return ret;
}


/*@}*/
