/**
 * @file   mks_handle_msg.c
 *
 * Coding of the mks_handle_msg function, which is called when the SWE
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
 *  11/19/2001     L Sollier    Create
 *
 *
 * (C) Copyright 2001 by Texas Instruments Incorporated, All Rights Reserved
 */

#include "mks/mks_api.h"
#include "mks/mks_i.h"
#include "mks/mks_messages_i.h"

#include "rv/rv_general.h"
#include "rvf/rvf_api.h"
#include "rvm/rvm_use_id_list.h"

/**
 * @name Functions implementation
 *
 */
/*@{*/

/**
 * function: mks_handle_msg
 */
T_RV_RET mks_handle_msg(T_RV_HDR   *msg_p)
{
   if (msg_p != NULL)
   {
      switch (msg_p->msg_id)
      {
         /* A new magic key sequence is added */
         case MKS_INFOS_KEY_SEQUENCE_MSG:
         {
            T_MKS_INFOS_KEY_SEQUENCE_MSG* msg_infos_key_sequence_p= (T_MKS_INFOS_KEY_SEQUENCE_MSG*) msg_p;

            MKS_SEND_TRACE("MKS: 'Add magic key sequence' message received", RV_TRACE_LEVEL_DEBUG_LOW);

            mks_add_key_sequence_i(msg_infos_key_sequence_p);
            break;
         }

         /* A magic key sequence is removed */
         case MKS_REMOVE_KEY_SEQUENCE_MSG:
         {
            T_MKS_REMOVE_KEY_SEQUENCE_MSG* msg_remove_key_sequence_p= (T_MKS_REMOVE_KEY_SEQUENCE_MSG*) msg_p;

            MKS_SEND_TRACE("MKS: 'Remove magic key sequence' message received", RV_TRACE_LEVEL_DEBUG_LOW);

            mks_remove_key_sequence_i(msg_remove_key_sequence_p);
            break;
         }


         /* Process a key released */
         case KPD_KEY_EVENT_MSG:
         {
            T_KPD_KEY_EVENT_MSG* msg_key_event_p= (T_KPD_KEY_EVENT_MSG*) msg_p;

            mks_check_key_sequence(msg_key_event_p);
            break;
         }

         /* Check the subscription status */
         case KPD_STATUS_MSG:
         {
            T_KPD_STATUS_MSG* msg_keypad_status_p= (T_KPD_STATUS_MSG*) msg_p;
            
            if (msg_keypad_status_p->operation == KPD_SUBSCRIBE_OP)
            {
               if (msg_keypad_status_p->status_value == KPD_PROCESS_OK)
               {
                  MKS_SEND_TRACE("MKS: Keypad subscription OK", RV_TRACE_LEVEL_DEBUG_LOW);
               }
               else
               {
                  MKS_SEND_TRACE("MKS: Keypad subscription failed", RV_TRACE_LEVEL_ERROR);
               }
            }
            else
            {
               MKS_SEND_TRACE("MKS: Unexpected keypad status message received", RV_TRACE_LEVEL_ERROR);
            }

            break;
         }

         default: 
         {
            /* Unknow message has been received */
            MKS_SEND_TRACE("MKS: Message received unknown", RV_TRACE_LEVEL_ERROR);
            break; 
         }
      }

      /* Free message */
      rvf_free_buf(msg_p);
   }

   return RV_OK;
}

/*@}*/
