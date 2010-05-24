/**
 * @file   lls_api.h
 *
 * API Definition for LLS (Low Level Servies) Service.
 *
 * This file gathers all the constants, structure and functions declaration
 * useful for a LLS service user.
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

#ifndef _LLS_API_H_
#define _LLS_API_H_


#include "rv/rv_general.h"
#include "chipset.cfg"

/**
 * @name External types
 *
 * Types used in API.
 *
 */
/*@{*/

/** Definition of available equipment, ... for D-Sample
 */
#if (ANLG_FAM == 11)

#include "bspTwl3029_Aux.h"
#define LLS_BACKLIGHT  BSP_TWL3029_AUX_LEDA
#define LLS_KEYPAD_LIGHT BSP_TWL3029_AUX_LEDB
#define LLS_SUBPANEL_LIGHT BSP_TWL3029_AUX_LEDC

#define LLS_KEYBKLIGHT_GPIO (2)

#else
#define LLS_LED_A         0x01
#define LLS_BACKLIGHT     0x02
#define LLS_PRECHARGE_LED 0x03
#endif
/*@}*/

#define PWL_LEVEL_REG (*(volatile unsigned char*) 0xFFFE8000)
#define PWL_CTRL_REG  (*(volatile unsigned char*) 0xFFFE8001)

typedef UINT8 T_LLS_EQUIPMENT;

typedef enum{
	LLS_BL_LEVEL_0 = 0,
	LLS_BL_LEVEL_1,
	LLS_BL_LEVEL_2,
	LLS_BL_LEVEL_3,
	LLS_BL_LEVEL_4,
	LLS_BL_LEVEL_5,
	LLS_BL_LEVEL_6,
	LLS_BL_LEVEL_7,
	LLS_BL_LEVEL_8,
	LLS_BL_LEVEL_9,
	LLS_BL_LEVEL_10,
	LLS_BL_LEVEL_11,
	LLS_BL_LEVEL_12,
	LLS_BL_LEVEL_13,
	LLS_BL_LEVEL_14,
	LLS_BL_LEVEL_15,
} T_LLS_BL_LEVEL;

/**
 * @name API functions
 *
 * API functions declarations.
 */
/*@{*/

/**
 * function: lls_switch_on
 *
 * This function switch on an equipment.
 *
 * @param   equipment_sort    equipment sort.
 * @return
 *    - RV_OK if operation is successful,
 *    - RV_INVALID_PARAMETER if one parameter is incorrect,
 *    - RV_NOT_SUPPORTED 
 *
 */
T_RV_RET lls_switch_on(T_LLS_EQUIPMENT equipment_sort, T_LLS_BL_LEVEL level);


/**
 * function: lls_switch_off
 *
 * This function switch off an equipment.
 *
 * @param   equipment_sort    equipment sort.
 * @return
 *    - RV_OK if operation is successful,
 *    - RV_INVALID_PARAMETER if one parameter is incorrect,
 *    - RV_NOT_SUPPORTED 
 *
 */
T_RV_RET lls_switch_off(T_LLS_EQUIPMENT equipment_sort);


/*@}*/


T_RV_RET lls_switch_getstatus(T_LLS_EQUIPMENT equipment_sort, T_LLS_BL_LEVEL *level);


#endif /* #ifndef _LLS_API_H_ */
