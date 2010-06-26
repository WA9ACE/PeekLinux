/**
 * @file  dcfg_api.h
 *
 * API Definition for DCFG SWE.
 *
 * @author  Jose Yp-Tcha (j-yp-tcha@ti.com)
 * @version 0.1
 */

/*
 * History:
 *
 *  Date         Author          Modification
 *  -------------------------------------------------------------------
 *  2/6/2003  Jose Yp-Tcha (j-yp-tcha@ti.com)    Create.
 *
 * (C) Copyright 2003 by Texas Instruments Incorporated, All Rights Reserved
 */

#ifndef __DCFG_API_H_
#define __DCFG_API_H_

#include "rvm_gen.h" /* Generic RVM types and functions. */

#ifdef __cplusplus
extern "C"
{
#endif

/**
 * @name DCFG Return type and return values.
 *
 * Currently they are the standard RV return types, but they may
 * be customized in the future.
 */
/*@{*/
typedef T_RV_RET T_DCFG_RETURN;

#define DCFG_OK RV_OK
#define DCFG_NOT_SUPPORTED RV_NOT_SUPPORTED
#define DCFG_MEMORY_ERR RV_MEMORY_ERR
#define DCFG_INTERNAL_ERR RV_INTERNAL_ERR
/*@}*/

/* bearer type shared by CONF et DCM */
typedef UINT8 T_BEARER_TYPE;
#define BESTONE_BEARER_TYPE 0
#define GPRS_BEARER_TYPE 0x01
#define GSM_BEARER_TYPE 0x02

#define DEFAULT_BESTONE_BEARER_TYPE (0xE0 | BESTONE_BEARER_TYPE)
#define DEFAULT_GPRS_BEARER_TYPE (0xE0 | GPRS_BEARER_TYPE)
#define DEFAULT_GSM_BEARER_TYPE (0xE0 | GSM_BEARER_TYPE)

/* IP User shared by the system */
typedef UINT8 T_APPLI_USER;
#define JAVA_USER 0

// ADD HERE NEW IP USERS

#define ALL_USER 0xff

typedef UINT8 T_DESC_ID; // 2 bits for the row, 1 for def, 5 for bearer

typedef struct
{
  T_APPLI_USER IPU_id;
  T_DESC_ID desc_id;
}T_CONF_ID;

/* Location of the IP User database informations */
#define CONF_DATA_USER_DEFAULT_BEARER ("/gsm/bearer/default_bearer")
#define CONF_DATA_USER_GPRS_BEARER ("/gsm/bearer/gprs_bearer")
#define CONF_DATA_USER_GSM_BEARER ("/gsm/bearer/gsm_bearer")

/**
 * @name API functions
 *
 * API functions declarations (bridge functions).
 */
/*@{*/

/*@}*/

#ifdef __cplusplus
}
#endif

#endif /*__DCFG_API_H_*/

