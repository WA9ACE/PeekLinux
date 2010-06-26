/*******************************************************************************\
*                                                                           	*
*  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION			*
*																				*
*  LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE			*
*	UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE		*
*	APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO		*
*	BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT		*
*	OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL			*
*	DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC.			*
*																				*
\*******************************************************************************/
/*******************************************************************************\
*
*   FILE NAME:      bthal_btdrv.c
*
*   DESCRIPTION:    Implementation of reset and shutdown sequences of BT Host
*					Controller chip.
*
*   AUTHOR:         Ordina - Rene Kuiken
*
\*******************************************************************************/


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include <nucleus.h>
#include "bthal_common.h"
#include "bthal_btdrv.h"
#include "typedefs.h"
#include "inth/sys_inth.h"
#include "btl_log.h"
#include "btl_defs.h"

BTL_LOG_SET_MODULE(BTL_LOG_MODULE_TYPE_HAL_BTDRV);


/* N Shutdown BRF6150 */
#define NSHUT_GPIO (UINT32) 37


/* External declarations needed for delay */
extern int btt_handle;

/* Run init only once */
static BOOL bt_drv_init = FALSE;

/********************************************************************************
 *
 * Function definitions
 *
 *******************************************************************************/
 
/*-------------------------------------------------------------------------------
 * BTHAL_BTDRV_Init()
 *
 *		Initializes BT Host Controller chip.
 */
BthalStatus BTHAL_BTDRV_Init()
{
	return(BTHAL_STATUS_SUCCESS);
}


/*-------------------------------------------------------------------------------
 * BTHAL_BTDRV_Reset()
 *
 *		Resets BT Host Controller chip.
 */
BthalStatus BTHAL_BTDRV_Reset()
{
	BTL_LOG_INFO(("Performing reset of BT Host Controller chip"));
    /* Configure GPIO hardware reset pin */
    AI_ConfigBitAsOutput((int)NSHUT_GPIO);

	/* Clear BT_nSHUTDOWN */
    AI_ResetBit((int)NSHUT_GPIO);

	/* Delay for 8 ms - The Actual sleep time measured is * ~4 */
	NU_Sleep(2);

	/* Set BT_nSHUTDOWN */
	AI_SetBit((int)NSHUT_GPIO);

	/* Delay for ~ 12 ms - The Actual sleep time measured is * ~4 */
	NU_Sleep(3);

	return BTHAL_STATUS_SUCCESS;
    
}

/*-------------------------------------------------------------------------------
 * BTHAL_BTDRV_Shutdown()
 *
 *		Shutdowns BT Host Controller chip.
 */
BthalStatus BTHAL_BTDRV_Shutdown()
{
	BTL_LOG_INFO(("Performing shutdown of BT Host Controller chip"));

	/* Clear BT_nSHUTDOWN */
    AI_ResetBit(NSHUT_GPIO);

	return BTHAL_STATUS_SUCCESS;
}

