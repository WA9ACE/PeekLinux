/**
 * @file   kpd_power_api.h
 *
 * API Definition for keypad driver, used bi PWR SWE.
 *
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

#ifndef _KPD_POWER_API_H_
#define _KPD_POWER_API_H_

#include "kpd/kpd_cfg.h"
#include "kpd/kpd_api.h"

#include "rv/rv_general.h"
#include "rvf/rvf_api.h"


/* This structure defines the two functions to call when PWR key is pressed */
typedef struct {  void (*pressed)(T_KPD_VIRTUAL_KEY_ID);
                  void (*released)(void);
               } T_KPD_KEYPAD;



#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name API functions
 *
 * API functions declarations.
 */
/*@{*/

/**
 * function: kpd_power_key_pressed
 *
 * This function is called by the PWR SWE when the PWR key is pressed.
 *
 * @return
 *    - RV_OK if operation is successfull,
 *    - RV_MEMORY_ERR if message could not be sent to the keypad task.
 *    - RV_INTERNAL_ERR if keypad driver is not yet initialized.
 *
 */
T_RV_RET kpd_power_key_pressed(BOOL);

#ifdef __cplusplus
}
#endif


#endif /* #ifndef _KPD_POWER_API_H_ */

