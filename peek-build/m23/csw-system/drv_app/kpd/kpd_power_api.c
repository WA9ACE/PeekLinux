/**
 * @file   kpd_power_api.c
 *
 * Implementation of bridge function for PWR SWE interface.
 *
 * @author   Laurent Sollier (l-sollier@ti.com)
 * @version 0.1
 */

/*
 * History:
 *
 *   Date          Author       Modification
 *  ----------------------------------------
 *  11/02/2001     L Sollier    Create
 *
 *
 * (C) Copyright 2001 by Texas Instruments Incorporated, All Rights Reserved
 */

#include "kpd/kpd_power_api.h"
#include "kpd/kpd_messages_i.h"
#include "kpd/kpd_physical_key_def.h"
#include "kpd/kpd_env.h"

#include "rvm/rvm_use_id_list.h"

/* External declaration */
extern T_KPD_ENV_CTRL_BLK* kpd_env_ctrl_blk;

/* This variable ius defined here but is used by the PWR SWE */
T_KPD_KEYPAD Kp = {0};

/**
 * @name Functions implementation
 *
 */
/*@{*/


/**
 * function: kpd_power_key_pressed
 */
#if (CHIPSET!=15)
T_RV_RET kpd_power_key_pressed(void)
{
   T_RV_RET ret = RV_OK;

   T_RVF_MB_STATUS mb_status;
   T_KPD_KEY_PRESSED_MSG* msg_key_pressed;

   /* Check if initialization has been correctly done */
   if ( (kpd_env_ctrl_blk == 0) || (kpd_env_ctrl_blk->swe_is_initialized == FALSE) )
   {
      KPD_SEND_TRACE("KPD: Initialization is not yet done or failed", RV_TRACE_LEVEL_ERROR);
      return RV_INTERNAL_ERR;
   }

   /* Reserve memory for message */
   mb_status = rvf_get_buf (kpd_env_ctrl_blk->prim_id, sizeof(T_KPD_KEY_PRESSED_MSG), (void **) &msg_key_pressed);

   if (mb_status == RVF_GREEN) /* Memory allocation success */
   {
      /* Fill the message */
      msg_key_pressed->hdr.msg_id = KPD_KEY_PRESSED_MSG;
      msg_key_pressed->value = KPD_SHORT_PRESS_PWR_KEY;
      msg_key_pressed->key_to_process = TRUE;

      /* Send message to the keypad task */
      rvf_send_msg(kpd_env_ctrl_blk->addr_id, msg_key_pressed);
   }
   else
   {
      KPD_SEND_TRACE("KPD: Memory allocation error", RV_TRACE_LEVEL_ERROR);
      if (mb_status == RVF_YELLOW)
         rvf_free_buf(msg_key_pressed);
      ret = RV_MEMORY_ERR;
   }


   return ret;
}

#else

T_RV_RET kpd_power_key_pressed(void)
{
   T_RV_RET ret = RV_OK;

   T_RVF_MB_STATUS mb_status;
   T_KPD_KEY_PRESSED_MSG* msg_key_pressed;

   /* Check if initialization has been correctly done */
   if ( (kpd_env_ctrl_blk == 0) || (kpd_env_ctrl_blk->swe_is_initialized == FALSE) )
   {
      KPD_SEND_TRACE("KPD: Initialization is not yet done or failed", RV_TRACE_LEVEL_ERROR);
      return RV_INTERNAL_ERR;
   }

   /* Reserve memory for message */
   mb_status = rvf_get_buf (kpd_env_ctrl_blk->prim_id, sizeof(T_KPD_KEY_PRESSED_MSG), (void **) &msg_key_pressed);

   if (mb_status == RVF_GREEN) /* Memory allocation success */
   {
      /* Fill the message */
      msg_key_pressed->hdr.msg_id = KPD_PROCESS_POWER_KEY_MSG;

      /* Send message to the keypad task */
      rvf_send_msg(kpd_env_ctrl_blk->addr_id, msg_key_pressed);
   }
   else
   {
      KPD_SEND_TRACE("KPD: Memory allocation error", RV_TRACE_LEVEL_ERROR);
      if (mb_status == RVF_YELLOW)
         rvf_free_buf(msg_key_pressed);
      ret = RV_MEMORY_ERR;
   }


   return ret;
}

#endif
/*@}*/
