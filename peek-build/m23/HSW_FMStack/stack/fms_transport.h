/*******************************************************************************\
##                                                                             *
##  TEXAS INSTRUMENTS ISRAEL PROPRIETARY AND CONFIDENTIAL INFORMATION          *
##                                                                             *
##  LEGAL NOTE: THE TRANSFER OF THE TECHNICAL INFORMATION IS BEING MADE        *
##	UNDER AN EXPORT LICENSE ISSUED BY THE ISRAELI GOVERNMENT AND THAT THE      *
##	APPLICABLE EXPORT LICENSE DOES NOT ALLOW THE TECHNICAL INFORMATION TO      *
##	BE USED FOR THE MODIFICATION OF THE BT ENCRYPTION OR THE DEVELOPMENT       *
##	OF ANY NEW ENCRYPTION. THE INFORMATION CAN BE USED FOR THE INTERNAL        *
##	DESIGN AND MANUFACTURE OF TI PRODUCTS THAT WILL CONTAIN THE BT IC          *
##                                                                             *
\*******************************************************************************/
/*******************************************************************************\
*
*   FILE NAME:      fms_transport.h
*
*   BRIEF:          This file defines the API to the relevant FM transport.
*
*   DESCRIPTION:    General
*
*                   This file defines procedures to communicate with the FM radio
*					through the relevant transport - UART or I2C.
*                   
*   AUTHOR:         Keren Gazit
*
\*******************************************************************************/

#ifndef __FMS_TRANSPORT_H
#define __FMS_TRANSPORT_H

/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "fm_types.h"

/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/

/*-------------------------------------------------------------------------------
 * TiTransportFmNotificationType enum
 *
 *     Defines FM notifications from the chip manager.
 */
typedef enum
{	
	TI_TRANSPORT_FM_NOTIFICATION_FM_ON_COMPLETE,
	TI_TRANSPORT_FM_NOTIFICATION_FM_OFF_COMPLETE
} TiTransportFmNotificationType;

/*-------------------------------------------------------------------------------
 * TiTrasportState enum
 *
 *     Defines transport state by the chip manager.
 */
typedef enum
{
	TI_TRANSPORT_STATE_CHIP_OFF,
	TI_TRANSPORT_STATE_BT_ON,
	TI_TRANSPORT_STATE_FM_ON,
	TI_TRANSPORT_STATE_ALL_ON
} TiTrasportState;

/*-------------------------------------------------------------------------------
 * TiTransportFmNotificationCb type
 *
 *     A function of this type is called for FM notifications from the chip manager.
 */
typedef void (*TiTransportFmNotificationCb)(TiTransportFmNotificationType notification);


/********************************************************************************
 *
 * Function declarations
 *
 *******************************************************************************/

 /*-------------------------------------------------------------------------------
 * fm_transport_sendWriteCommand()
 *
 * Brief:  
 *		Sends a write command to the BRF6350.
 *
 * Description:
 *    Sends an FM write command to the BRF6350 through the relevant transport.
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Parameters:
 *		void.
 *
 * Returns:
 *		FM_STATUS_PENDING - FM command was sent and waiting for command complete.
 *
 *		Other FM_STATUS -  Failed sending the command.
 */
FmStatus fm_transport_sendWriteCommand(void);

/*-------------------------------------------------------------------------------
 * fm_transport_sendPowerModeCommand()
 *
 * Brief:  
 *		Sends a power mode command to the BRF6350.
 *
 * Description:
 *    Sends an FM power mode command to the BRF6350 through the relevant transport.
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Parameters:
 *		void.
 *
 * Returns:
 *		FM_STATUS_PENDING - FM command was sent and waiting for command complete.
 *
 *		Other FM_STATUS -  Failed sending the command.
 */
FmStatus fm_transport_sendPowerModeCommand(void);

/*-------------------------------------------------------------------------------
 * fm_transport_sendInitCmd()
 *
 * Brief:  
 *		Sends FM init command to the BRF6350.
 *
 * Description:
 *    Sends an FM init command to the BRF6350 through the relevant transport.
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Parameters:
 *		void.
 *
 * Returns:
 *		FM_STATUS_PENDING - FM command was sent and waiting for command complete.
 *
 *		Other FM_STATUS -  Failed sending the command.
 */
FmStatus fm_transport_sendInitCmd(TIFM_U16 hci_opcode, TIFM_U8 len);

/*-------------------------------------------------------------------------------
 * fm_transport_sendReadStatus()
 *
 * Brief:  
 *		Sends FM read command to the BRF6350.
 *
 * Description:
 *    Sends an FM read command to the BRF6350 through the relevant transport.
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Parameters:
 *		void.
 *
 * Returns:
 *		FM_STATUS_PENDING - FM command was sent and waiting for command complete.
 *
 *		Other FM_STATUS -  Failed sending the command.
 */
FmStatus fm_transport_sendReadStatus(void);

/*-------------------------------------------------------------------------------
 * fm_transport_sendFmReadRds()
 *
 * Brief:  
 *		Sends FM read RDS command to the BRF6350.
 *
 * Description:
 *    Sends an FM read RDS command to the BRF6350 through the relevant transport.
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Parameters:
 *		void.
 *
 * Returns:
 *		FM_STATUS_PENDING - FM command was sent and waiting for command complete.
 *
 *		Other FM_STATUS -  Failed sending the command.
 */
FmStatus fm_transport_sendFmReadRds(void);

/*-------------------------------------------------------------------------------
 * fm_transport_sendReadInt()
 *
 * Brief:  
 *		Sends FM read flag/mask command to the BRF6350.
 *
 * Description:
 *    Sends an FM read flag or mask command to the BRF6350 through the relevant transport.
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Parameters:
 *		fm_opcode [in] - which part of the interrupt to read - flag or mask. FLAG_GET or INT_MASK_SET_GET.
 *
 * Returns:
 *		FM_STATUS_PENDING - FM command was sent and waiting for command complete.
 *
 *		Other FM_STATUS -  Failed sending the command.
 */
FmStatus fm_transport_sendReadInt(TIFM_U8 fm_opcode);

/*-------------------------------------------------------------------------------
 * fm_transport_init()
 *
 * Brief:  
 *		Chip manager initialization.
 *
 * Description:
 *    Initialize chip manager and register FM for notifications.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		callback [in] - the callback for FM notifications.
 *
 * Returns:
 *		FM_STATUS_SUCCESS - Chip manager initialized and FM callback registered successfully.
 *
 *		Other -  Failed.
 */
FmStatus		fm_transport_init(TiTransportFmNotificationCb callback);

/*-------------------------------------------------------------------------------
 * fm_transport_on()
 *
 * Brief:  
 *	  Power On the FM using the chip manager's interface.
 *
 * Description:
 *    Powering On the FM must be done through the chip manager to make sure
 *	  that the chip is ON and the BT init script was sent.
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Parameters:
 *		void.
 *
 * Returns:
 *		FM_STATUS_SUCCESS - the chip is powered on and the fm power on can start.
 *
 *		FM_STATUS_PENDING - the chip needs to be powered on and when finished the
 *							FM callback will be called and the FM will be powered on.
 */
FmStatus		fm_transport_on();

/*-------------------------------------------------------------------------------
 * fm_transport_off()
 *
 * Brief:  
 *	  Update the chip manager of powering off the fm.
 *
 * Description:
 *    After powering off the FM update the chip manager so it will power off 
 *	  the chip if necessary.
 *
 * Type:
 *		Asynchronous/Synchronous
 *
 * Parameters:
 *		void.
 *
 * Returns:
 *		FM_STATUS_SUCCESS - the chip is powered off.
 *
 *		FM_STATUS_PENDING - The chip needs to be powered off and when finished the
 *							FM callback will be called.
 */
FmStatus		fm_transport_off();

/*-------------------------------------------------------------------------------
 * fm_transport_getState()
 *
 * Brief:  
 *	  Gets the chip manager's state.
 *
 * Description:
 *    Checks the chip manager's state.
 *
 * Type:
 *		Synchronous
 *
 * Parameters:
 *		void.
 *
 * Returns:
 *		TI_TRANSPORT_STATE_CHIP_OFF - the chip is powered off.
 *
 *		TI_TRANSPORT_STATE_BT_ON - The BT is on.
 *
 *		TI_TRANSPORT_STATE_FM_ON - The FM is on.
 *
 *		TI_TRANSPORT_STATE_ALL_ON - Both BT and FM are on.
 *
 */
TiTrasportState fm_transport_getState();


#endif
