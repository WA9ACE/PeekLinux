/**
 * @file   lls_functions.c
 *
 * Implementation of LLS functions.
 *
 * @author   Laurent Sollier (l-sollier@ti.com)
 * @version 0.1
 */

/*
 * History:
 *
 *   Date          Author       Modification
 *  ----------------------------------------
 *  03/12/2002     L Sollier    Create
 *
 *
 * (C) Copyright 2002 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef _WINDOWS
   #include "l1sw.cfg"
   #include "chipset.cfg"
#endif

#include "lls/lls_i.h"
#include "lls/lls_api.h"
#include "lls/lls_env.h"
#include "spi/spi_api.h"
#include "rvf/rvf_api.h"


/* Number of available equipment's */
#define NUMBER_OF_EQUIPMENT 3

/* Parameters for each EQUIPMENT */
typedef struct
{
   T_LLS_EQUIPMENT  equipment_sort;
   UINT16			page;
   UINT16			address;
   UINT8            bit;
} T_EQUIPMENT_PARAM;

/* Table of parameters for all the equipment's */
typedef T_EQUIPMENT_PARAM T_EQUIPMENT_PARAM_TABLE[NUMBER_OF_EQUIPMENT];

/* Definition of the parameters for the equipment */
#if (ANLG_FAM == 2)
   static T_EQUIPMENT_PARAM_TABLE equipment_param_table = 
   {  {LLS_LED_A,         PAGE1, AUXLED,  0},
      {LLS_BACKLIGHT,     PAGE1, AUXLED,  1},
      {LLS_PRECHARGE_LED, PAGE0, BCICTL2, 5}
   };
#else
   static T_EQUIPMENT_PARAM_TABLE equipment_param_table = {0};
#endif

/* Save of the action to perform */
typedef struct
{
   T_LLS_EQUIPMENT   equipment_index;
   UINT8             action;
} T_ACTION;

static T_ACTION action_to_perform = {0};

extern T_LLS_ENV_CTRL_BLK* lls_env_ctrl_blk;

/* Mutex used to protect perform only one action simultaneously */

/**
 * @name Functions implementation
 *
 */
/*@{*/

/**
 * function: lls_initialize
 */
T_RV_RET lls_initialize(void)
{
   T_RV_RET ret = RV_OK;
   UINT8 i;

   /* Mutex initialization */
   ret = rvf_initialize_mutex(&(lls_env_ctrl_blk->mutex));
   if (ret != RVF_OK)
      return RV_INTERNAL_ERR;

   /* Initialisation of the equipment at SWITCH_OFF */
#if(ANLG_FAM == 11)
   ret = lls_switch_off(LLS_BACKLIGHT);
   ret = lls_switch_off(LLS_KEYPAD_LIGHT);
   ret = lls_switch_off(LLS_SUBPANEL_LIGHT);
   if (ret != RV_OK)
         return RV_INTERNAL_ERR;
#else 
for (i = 0; i < NUMBER_OF_EQUIPMENT; i++)
   {
      ret = lls_switch_off(equipment_param_table[i].equipment_sort);
      if (ret != RV_OK)
         return RV_INTERNAL_ERR;
   }
#endif
   return RV_OK;
}


/**
 * function: lls_kill_service
 */
T_RV_RET lls_kill_service(void)
{
   T_RV_RET ret = RV_OK;
   ret = rvf_delete_mutex(&(lls_env_ctrl_blk->mutex));
   if (ret != RVF_OK)
      return RV_INTERNAL_ERR;

   return ret;
}

#if(ANLG_FAM != 11)
/**
 * function: lls_callback_equipment_status
 */
void lls_callback_equipment_status(UINT16* equipment_status)
{
   T_RV_RET ret = RV_OK;
   UINT16 new_equipment_status = *equipment_status;

   LLS_SEND_TRACE_PARAM("LLS: Received equipment status", (UINT32) new_equipment_status, RV_TRACE_LEVEL_DEBUG_LOW);

   if (action_to_perform.action == SWITCH_ON)
      new_equipment_status |= 1 << equipment_param_table[action_to_perform.equipment_index].bit;
   else
      new_equipment_status &= ~(1 << equipment_param_table[action_to_perform.equipment_index].bit);

   LLS_SEND_TRACE_PARAM("LLS: New equipment status", (UINT32) new_equipment_status, RV_TRACE_LEVEL_DEBUG_LOW);

   ret = spi_abb_write(equipment_param_table[action_to_perform.equipment_index].page,
                 equipment_param_table[action_to_perform.equipment_index].address,
                 new_equipment_status);

   if (ret != RV_OK)
      LLS_SEND_TRACE("LLS: Error on SPI read",  RV_TRACE_LEVEL_ERROR);


   rvf_unlock_mutex(&(lls_env_ctrl_blk->mutex));
}

/**
 * function: lls_manage_equipment
 */
T_RV_RET lls_manage_equipment(UINT8 equipment_index, UINT8 action)
{
   T_RV_RET ret = RV_OK;

   /* Lock mutex until response from SPI is received */
   rvf_lock_mutex(&(lls_env_ctrl_blk->mutex));

   /* Save action to do */
   action_to_perform.equipment_index = equipment_index;
   action_to_perform.action = action;

   ret = spi_abb_read(equipment_param_table[equipment_index].page,
                equipment_param_table[equipment_index].address,
                lls_callback_equipment_status);

   if (ret != RV_OK)
      LLS_SEND_TRACE("LLS: Error on SPI read",  RV_TRACE_LEVEL_ERROR);

   return ret;
}

/**
 * function: lls_search_index
 */
T_RV_RET lls_search_index(T_LLS_EQUIPMENT equipment, UINT8* equipment_index)
{
   T_RV_RET ret = RV_OK;
   UINT8 i;

   for (i = 0; i < NUMBER_OF_EQUIPMENT; i++)
      if (equipment == equipment_param_table[i].equipment_sort)
      {
         *equipment_index = i;
         return RV_OK;
      }

   return RV_INVALID_PARAMETER;
}
#endif


/*@}*/
