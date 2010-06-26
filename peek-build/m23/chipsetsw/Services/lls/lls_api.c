/**
 * @file   lls_api.c
 *
 * Implementation of Functions.
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

#include "lls/lls_api.h"
#include "lls/lls_env.h"
#include "lls/lls_i.h"

#if ((ANLG_FAM == 3) || (ANLG_FAM == 4))
  #include "sys_types.h" /* needed for buzzer.h */
  #include "buzzer/buzzer.h"
#endif

/** External declaration */
extern T_LLS_ENV_CTRL_BLK* lls_env_ctrl_blk;

#define LLS_ON_DAC_REG_VALUE 0xB5 /*Analog output=60mA, duty cycle=5/16 */
#define LLS_ON_CTRL_REG_VALUE 0x08 /*default value */
#define LLS_OFF_DAC_REG_VALUE 0x00 /*Analog output=0mA, duty cycle=0 */
#define LLS_OFF_CTRL_REG_VALUE 0x08 /*default value */

/**
 * @name Functions implementation
 *
 */
/*@{*/


/**
 * function: lls_switch_on
 */
T_RV_RET lls_switch_on(T_LLS_EQUIPMENT equipment_sort)
{
   BspTwl3029_ReturnCode ret_val;
   T_RV_RET ret = RV_OK;
   #if (ANLG_FAM == 2)
   UINT8 equipment_index;

   /* Check if initialization has been correctly done */
   if ( (lls_env_ctrl_blk == 0) || (lls_env_ctrl_blk->swe_is_initialized == FALSE) )
   {
      LLS_SEND_TRACE("LLS: Initialization is not yet done or failed", RV_TRACE_LEVEL_ERROR);
      return RV_INTERNAL_ERR;
   }

   LLS_SEND_TRACE_PARAM("LLS: Switch ON request for equipment", equipment_sort, RV_TRACE_LEVEL_DEBUG_HIGH);

   /* Retrieve index of the equipment in the table */
   ret = lls_search_index(equipment_sort, &equipment_index);

   if (ret != RV_OK)
   {
      LLS_SEND_TRACE("LLS: Equipment is unknow", RV_TRACE_LEVEL_ERROR);
      return ret;
   }

   ret = lls_manage_equipment(equipment_index, SWITCH_ON);

   return ret;

#elif ((ANLG_FAM == 3) || (ANLG_FAM == 4))
   LT_Enable();
   return RV_OK;
#elif (ANLG_FAM == 11)
   /* Check if initialization has been correctly done */
   if ( (lls_env_ctrl_blk == 0) || (lls_env_ctrl_blk->swe_is_initialized == FALSE) )
   {
      LLS_SEND_TRACE("LLS: Initialization is not yet done or failed", RV_TRACE_LEVEL_ERROR);
      return RV_INTERNAL_ERR;
   }
    rvf_lock_mutex(&(lls_env_ctrl_blk->mutex));
    ret_val=bspTwl3029_Aux_LedEnable(NULL,equipment_sort,BSP_TWL3029_LED_ENABLE);
	if (ret_val==BSP_TWL3029_RETURN_CODE_FAILURE) {
      ret=RV_INTERNAL_ERR;
      }
    else ret=RV_OK; 
    if (ret)
		{
	   	LLS_SEND_TRACE_PARAM("LLS: bspTwl3029_Aux_LedEnable failed for equipment%d", equipment_sort,RV_TRACE_LEVEL_ERROR);		
		rvf_unlock_mutex(&(lls_env_ctrl_blk->mutex));
		return RV_INTERNAL_ERR;
		}
	ret_val=bspTwl3029_Aux_LedConfig(NULL,equipment_sort,LLS_ON_DAC_REG_VALUE,LLS_ON_CTRL_REG_VALUE);
if (ret_val==BSP_TWL3029_RETURN_CODE_FAILURE) {
      ret=RV_INTERNAL_ERR;
      }
else ret=RV_OK; 
    if (ret)
 	    {
   	     LLS_SEND_TRACE_PARAM("LLS: bspTwl3029_Aux_LedConfig failed for equipment%d", equipment_sort,RV_TRACE_LEVEL_ERROR);		
	     rvf_unlock_mutex(&(lls_env_ctrl_blk->mutex));
		 return RV_INTERNAL_ERR;
		}
    rvf_unlock_mutex(&(lls_env_ctrl_blk->mutex));
	return ret;
#else
   return RV_NOT_SUPPORTED;
#endif
}

/**
 * function: lls_switch_off
 *
 */
T_RV_RET lls_switch_off(T_LLS_EQUIPMENT equipment_sort)
{
    BspTwl3029_ReturnCode ret_val; 
    T_RV_RET ret = RV_OK;
#if (ANLG_FAM == 2)
   UINT8 equipment_index;

   /* Check if initialization has been correctly done */
   if ( (lls_env_ctrl_blk == 0) || (lls_env_ctrl_blk->swe_is_initialized == FALSE) )
   {
      LLS_SEND_TRACE("LLS: Initialization is not yet done or failed", RV_TRACE_LEVEL_ERROR);
      return RV_INTERNAL_ERR;
   }

   LLS_SEND_TRACE_PARAM("LLS: Switch OFF request for equipment", equipment_sort, RV_TRACE_LEVEL_DEBUG_HIGH);

   /* Retrieve index of the equipment in the table */
   ret = lls_search_index(equipment_sort, &equipment_index);

   if (ret != RV_OK)
   {
      LLS_SEND_TRACE("LLS: Equipment is unknow", RV_TRACE_LEVEL_ERROR);
      return ret;
   }

   ret = lls_manage_equipment(equipment_index, SWITCH_OFF);
   
   return ret;

#elif ((ANLG_FAM == 3) || (ANLG_FAM == 4))
   LT_Disable();

   return RV_OK;
#elif (ANLG_FAM == 11)
   /* Check if initialization has been correctly done */
   if ( (lls_env_ctrl_blk == 0) || (lls_env_ctrl_blk->swe_is_initialized == FALSE) )
   {
      LLS_SEND_TRACE("LLS: Initialization is not yet done or failed", RV_TRACE_LEVEL_ERROR);
      return RV_INTERNAL_ERR;
   }
    rvf_lock_mutex(&(lls_env_ctrl_blk->mutex));
	ret_val=bspTwl3029_Aux_LedConfig(NULL,equipment_sort,LLS_OFF_DAC_REG_VALUE,LLS_OFF_CTRL_REG_VALUE);
if (ret_val==BSP_TWL3029_RETURN_CODE_FAILURE) {
      ret=RV_INTERNAL_ERR;
      }
    
else ret=RV_OK; 	
    if (ret)
		{
	     LLS_SEND_TRACE_PARAM("LLS: bspTwl3029_Aux_LedConfig failed for equipment%d", equipment_sort,RV_TRACE_LEVEL_ERROR);		
         rvf_unlock_mutex(&(lls_env_ctrl_blk->mutex));
		 return RV_INTERNAL_ERR;
		}

    ret_val=bspTwl3029_Aux_LedEnable(NULL,equipment_sort,BSP_TWL3029_LED_DISABLE);
    if (ret_val==BSP_TWL3029_RETURN_CODE_FAILURE) {
      ret=RV_INTERNAL_ERR;
      }
    
else ret=RV_OK; 
    if (ret)
       	{
   	     LLS_SEND_TRACE_PARAM("LLS: bspTwl3029_Aux_LedEnable failed for equipment%d", equipment_sort,RV_TRACE_LEVEL_ERROR);		
		 rvf_unlock_mutex(&(lls_env_ctrl_blk->mutex));
		 return RV_INTERNAL_ERR;
		}
	rvf_unlock_mutex(&(lls_env_ctrl_blk->mutex));
	return ret;

#else
   return RV_NOT_SUPPORTED;
#endif
}


/*@}*/
