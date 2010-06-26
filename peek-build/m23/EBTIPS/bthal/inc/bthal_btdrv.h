/*******************************************************************************\
##                                                                           	*
##  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION			*
##																				*
##  LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE			*
##	UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE		*
##	APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO		*
##	BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT		*
##	OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL			*
##	DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC.			*
##																				*
\*******************************************************************************/
/*******************************************************************************\
*
*   FILE NAME:		bthal_btdrv.h
*
*	BRIEF:          Type definitions and function prototypes for the
*					bthal_btdrv.c module implementing reset and shutdown of the
*					BT Host Controller chip.
*
*   DESCRIPTION:    General
*
*					Type definitions and function prototypes for the
*					bthal_btdrv.c module implementing reset and shutdown of the
*					BT Host Controller chip.
*
*   AUTHOR:			V. Abram
*
\*******************************************************************************/

#ifndef __BTHAL_BTDRV_H
#define __BTHAL_BTDRV_H

 
/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include <bthal_common.h>


/********************************************************************************
 *
 * Function declarations
 *
 *******************************************************************************/


/*-------------------------------------------------------------------------------
 * BTHAL_BTDRV_Init()
 *
 *	Brief:  
 *		Initialize the BTHAL BTDRV.
 *
 *	Description:
 *		Initialize the BTHAL BTDRV.
 *		This function should only be called once at system-startup.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		None.
 *
 * Returns:
 *		BTHAL_STATUS_SUCCESS - Operation is successful.
 *
 *		BTHAL_STATUS_FAILED - The operation failed.
 */
BthalStatus BTHAL_BTDRV_Init();


/*-------------------------------------------------------------------------------
 * BTHAL_BTDRV_Reset()
 *
 *	Brief:  
 *		Performs reset to BT Host Controller chip.
 *
 *	Description:
 *		Performs reset to BT Host Controller chip.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	None.
 *
 * Returns:
 *		BTHAL_STATUS_SUCCESS - if the resetting has been successfully	finished.
 *
 *		BTHAL_STATUS_FAILED or any specific error defined in BthalStatus type,
 *			if the resetting failed.
 */
BthalStatus BTHAL_BTDRV_Reset(void);


/*-------------------------------------------------------------------------------
 * BTHAL_BTDRV_Shutdown()
 *
 *	Brief:  
 *		Performs shutdown to BT Host Controller chip.
 *
 *	Description:
 *		Performs shutdown to BT Host Controller chip.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	None.
 *
 * Returns:
 *		BTHAL_STATUS_SUCCESS - if the shutdown has been successfully	finished.
 *
 *		BTHAL_STATUS_FAILED or any specific error defined in BthalStatus type,
 *			if the shutdown failed.
 */
BthalStatus BTHAL_BTDRV_Shutdown(void);


#endif /* __BTHAL_BTDRV_H */




