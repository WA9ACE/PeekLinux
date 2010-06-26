/**
 * @file   lls_i.h
 *
 * Declaration of internal function for LLS service.
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

#ifndef _LLS_I_H_
#define _LLS_I_H_

#include "lls/lls_api.h"

#include "rv/rv_general.h"
#include "rvm/rvm_use_id_list.h"


/* Macro definition for sending trace */
#define LLS_SEND_TRACE(text,level) rvf_send_trace(text, sizeof(text)-1, NULL_PARAM, level, LLS_USE_ID )
#define LLS_SEND_TRACE_PARAM(text,param,level) rvf_send_trace(text, sizeof(text)-1, param, level, LLS_USE_ID )


/* Definition of ON/OFF for an equipment */
#define SWITCH_ON  1
#define SWITCH_OFF 0


/**
 * @name Internal functions
 *
 */
/*@{*/

/**
 * function: lls_callback_equipment_status
 *
 * This function is called by SPI task to get the status of the equipment.
 *
 * @param   equipment_status  equipment status.
 *
 */
void lls_callback_equipment_status(UINT16* equipment_status);

/**
 * function: lls_initialize
 *
 * This function is called by Riviera environment to initialize the SWE.
 *
 *
 * @return
 *    - RV_OK if operation is successfull
 *    - RV_INTERNAL_ERR if an internal error occur
 */
T_RV_RET lls_initialize(void);

/**
 * function: lls_kill_service
 *
 * This function is called by Riviera environment to kill the SWE.
 *
 *
 * @return
 *    - RV_OK if operation is successfull
 *    - RV_INTERNAL_ERR if an internal error occur
 */
T_RV_RET lls_kill_service(void);

/**
 * function: lls_manage_equipment
 *
 * This function manage the switch ON /switch OFF of the equipment.
 *
 * @param   equipment_index   index of the equipment in the table of the availabble equipments.
 * @param   action            Action to perform on the equipment (switch ON or switch OFF).
 *
 * @return
 *    - RV_OK if operation is successfull
 */
T_RV_RET lls_manage_equipment(UINT8 equipment_index, UINT8 action);

/**
 * function: lls_search_index
 *
 * This function retrieve the index of the equipment in the table from the equipment number
 * defined in the API
 *
 * @param   equipment               Equipment number as defined in the API.
 * @param   equipment_index (OUT)   Action to perform on the equipment (switch ON or switch OFF).
 *
 * @return
 *    - RV_OK if operation is successfull
 *    - RV_INVALID_PARAMETER if the equipment number doesn't exist.
 */
T_RV_RET lls_search_index(T_LLS_EQUIPMENT equipment, UINT8* equipment_index);


/*@}*/


#endif 



