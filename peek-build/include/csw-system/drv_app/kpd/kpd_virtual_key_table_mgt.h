/**
 * @file   kpd_virtual_key_table_mgt.h
 *
 * Declarations of the access functions to configuration keypad table. 
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

#ifndef _KPD_VIRTUAL_KEY_TABLE_MGT_H_
#define _KPD_VIRTUAL_KEY_TABLE_MGT_H_

#include "kpd/kpd_api.h"
#include "kpd/kpd_scan_functions.h"
#include "kpd/kpd_physical_key_def.h"

#include "rv/rv_general.h"

/** Definition for the parameter position in the function declared below when
 * the virtual key in the array is unknown.
 */
#define KPD_POS_NOT_AVAILABLE (-1)

/**
 * @name Acces functions to configuration table
 */
/*@{*/


/**
 * function: kpd_initialize_ascii_table
 *
 * This function initialize the ASCII table in order to improve performance.
 *
 * @return  RV_OK.
 */
T_RV_RET kpd_initialize_ascii_table(void);


/**
 * function: kpd_get_virtual_key
 *
 * This function retrieve the virtual key ID associated to the physical key id and the mode.
 * 
 *
 * @param   key_id      Physical key Id.
 * @param   mode        Mode associated.
 * @return  Virtual key Id or KPD_KEY_NULL if the physical key is not mapped.
 *
 */
T_KPD_VIRTUAL_KEY_ID kpd_get_virtual_key( T_KPD_PHYSICAL_KEY_ID key_id,
                                          T_KPD_MODE mode);

/**
 * function: kpd_get_ascii_key_value
 *
 * This function retrieve the ASCII code associated to the virtual key ID and the mode.
 *
 * @param   key_id         Physical key Id.
 * @param   mode           Mode associated : default or alphanumeric
 *                         (No verification is done on the value).
 * @param   ascii_code_pp  ASCII code (OUT), By default, return value is the empty string "".
 * @return  RV_OK.
 *
 */
T_RV_RET kpd_get_ascii_key_value(T_KPD_PHYSICAL_KEY_ID key_id,
                                 T_KPD_MODE mode,
                                 char** ascii_code_pp);

/**
 * function: kpd_retrieve_virtual_key_position
 *
 * This function retrieve the position in the array of virtual keys according to the mode.
 *
 *
 * @param   key_id         Virtual key Id.
 * @param   mode           Mode in which is searched the virtual key
 *                         (No verification is done on the value)
 * @param   position_p     Position of the virtual key Id in the array. If the virtual
 *                         doesn't exist, the returned value is KPD_POS_NOT_AVAILABLE (OUT)
 * @return
 *    - RV_OK if operation is successful.
 *    - RV_INVALID_PARAMETER if the virtual key doesn't exist.
 *
 */
T_RV_RET kpd_retrieve_virtual_key_position(  T_KPD_VIRTUAL_KEY_ID key_id,
                                             T_KPD_MODE mode,
                                             INT8* position_p);

/**
 * function: kpd_get_default_keys
 *
 * This function retrieve the position in the array of virtual keys according to the mode.
 *
 *
 * @param   available_keys_p  Table of available keys in default mode.
 * @return  RV_OK.
 *
 */
T_RV_RET kpd_get_default_keys( T_KPD_VIRTUAL_KEY_TABLE* available_keys_p);


/**
 * function: kpd_check_key_table
 *
 * This function check if the all the keys defined in keys_table are available
 * in the requested mode.
 * Note that for facilities, if number of keys are KPD_NB_PHYSICAL_KEYS, the table
 * is automatically fulfilled with all the virtual keys.
 *
 *
 * @param   keys_table_p   Table of keys.
 * @param   mode           Mode requested.
 * @return  - RV_OK if operation successfull,
 *          - RV_INVALID_PARAMETER if at least one key is not defined in the requested mode.
 *
 */
T_RV_RET kpd_check_key_table(T_KPD_VIRTUAL_KEY_TABLE* keys_table_p, T_KPD_MODE mode);

/**
 * function: kpd_define_new_config
 *
 * This function define a link between reference keys (virtual keys in default mode),
 * and configurable keys. The virtual key Id of the configurable key can be defined
 * by the user in another part of the source code.
 *
 * @param   reference_keys_p  Table of reference keys.
 * @param   new_keys_p        Tabe of keys to define in new mode.
 * @return RV_OK.
 *
 */
void kpd_define_new_config(T_KPD_VIRTUAL_KEY_TABLE* reference_keys_p,
                           T_KPD_VIRTUAL_KEY_TABLE* new_keys_p);

/**
 * function: kpd_vpm_table_is_valid
 *
 * This function check the vpm table. Indeed, the physical keys must be defined
 * from 0 to KPD_NB_PHYSICAL_KEYS consecutively, and their position in the table
 * equal to the value. For more explanations, see vpm_table definition.
 *
 * @return
 *    - TRUE if the table is valid.
 *    - FALSE if the table is invalid.
 *
 */
BOOL kpd_vpm_table_is_valid(void);

/*@}*/


#endif /* #ifndef _KPD_VIRTUAL_KEY_TABLE_MGT_H_ */

