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
*   FILE NAME:		bthal_pm.h
*
*   BRIEF:          This file defines the API of the BTHAL PM module implementing 
*					participation of BT Host in platform's power management mechanism.
*
*   DESCRIPTION:    General
*
 * BTHAL PM API layer
 *
 *     	The BTHAL PM module is a set of functions used by the BT to participate 
 *		in the platform power management mechanism.
 *
 *		In general, there are two common PM schemas:
 *
 *		1) Voting, in which each module (participating in the decision) must vote
*		            in favor or against going to sleep in the platform power manager.
 *
 *		2) Polling, in which the platform power manager polls from time to time 
 *		each and every module (participating in the decision) if it can go to sleep.
 *
 *		This module keeps track of the current BT sleep decision about different 
*		            peripheral devices, and informs the platform power manager according to 
 *		the platform power management schema.
*                   
*   AUTHOR:         Ilan Elias
*
\*******************************************************************************/

#ifndef __BTHAL_PM_H
#define __BTHAL_PM_H

 
/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include <bthal_common.h>


/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BthalPmPeripheralDeviceMask type
 *
 *     Defines peripheral device. Can be OR'ed together.
 */
typedef BTHAL_U8 BthalPmPeripheralDeviceMask;

/* UART */
#define BTHAL_PM_PERIPHERAL_DEVICE_UART							(0x01)

/* I2C */
#define BTHAL_PM_PERIPHERAL_DEVICE_I2C							(0x02)

/* SDIO */
#define BTHAL_PM_PERIPHERAL_DEVICE_SDIO							(0x04)

/* All peripheral devices */
#define BTHAL_PM_PERIPHERAL_DEVICE_ALL							(0xFF)


/*-------------------------------------------------------------------------------
 * BthalPmSleepState type
 *
 *     Defines sleep state of a peripheral device.
 */
typedef BTHAL_U8 BthalPmSleepState;

/* Device can NOT go to sleep */
#define BTHAL_PM_SLEEP_STATE_AWAKE								(0x00)

/* Device can go to sleep */
#define BTHAL_PM_SLEEP_STATE_ASLEEP								(0x01)


/********************************************************************************
 *
 * Function declarations
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * BTHAL_PM_Init()
 *
 * Brief:  
 *		Performs initialization of participation of BT Host Controller in
 *		platform's power management mechanism.
 *
 * Description:
 *		Performs initialization of participation of BT Host Controller in
 *		platform's power management mechanism.
 *
 * Type:
 *		Synchronous or Asynchronous.
 *
 * Parameters:
 *     	None.
 *
 * Returns:
 *		BTHAL_STATUS_SUCCESS - if the initialization has been successfully
 *			finished.
 *
 *		BT_STATUS_PENDING - Operation started successfully. Completion will 
 *              be signaled via an event to the callback
 *
 *		BTHAL_STATUS_FAILED or any specific error defined in BthalStatus type,
 *			if the initialization failed.
 */
BthalStatus BTHAL_PM_Init(BthalCallBack	callback);


/*-------------------------------------------------------------------------------
 * BTHAL_PM_Deinit()
 *
 * Brief:  
 *		Performs deinitialization of participation of BT Host Controller in
 *		platform's power management mechanism.
 *
 * Description:
 *		Performs deinitialization of participation of BT Host Controller in
 *		platform's power management mechanism.
 *
 * Type:
 *		Synchronous or Asynchronous.
 *
 * Parameters:
 *     	None.
 *
 * Returns:
 *		BTHAL_STATUS_SUCCESS - if the deinitialization has been successfully
 *			finished.
 *
 *		BT_STATUS_PENDING - Operation started successfully. Completion will be 
 *              signaled via an event to the callback
 *
 *		BTHAL_STATUS_FAILED or any specific error defined in BthalStatus type,
 *			if the deinitialization failed.
 */
BthalStatus BTHAL_PM_Deinit(void);


/*-------------------------------------------------------------------------------
 * BTHAL_PM_ChangeSleepState()
 *
 * Brief:  
 *		Indicates the given peripheral device(s) can go to sleep or wake up.
 *
 * Description:
 *		Indicates the given peripheral device(s) can go to sleep or wake up.
 *
 *		Voting schema - the implementation should vote in favor of / against going 
 *						to sleep in the platform power manager.
 *
 *		Polling schema - the implementation should keep track of the current sleep 
 *						 state and reply with the current sleep state when the 
 *						 platform power manager demands it.
 *
 *		This might require preparing the device(s) before going to sleep or waking up.
 *		For example, before going to sleep, the UART RX pin might be muxed to a 
 *		certain	dedicated GPIO, on which the host will be able to wake up.
 *		When waking up, the UART RX normal path should be restored.
 *
 * Type:
 *		Synchronous.
 *
 * Parameters:
 *     	deviceMask [in] - peripheral device(s) which can go to sleep.
 *
 *		sleepState [in] - current sleep state of the given peripheral device(s).
 *
 * Returns:
 *		BTHAL_STATUS_SUCCESS - the current sleep state was changed successfully.
 *
 *		BTHAL_STATUS_FAILED - the current sleep state failed to change.
 */
BthalStatus BTHAL_PM_ChangeSleepState(BthalPmPeripheralDeviceMask deviceMask, 
										BthalPmSleepState sleepState);


#endif /* __BTHAL_PM_H */


