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
*   FILE NAME:      BTL_BMG_DDB.h
*
*   DESCRIPTION:  
* Device Database API layer
*
*	This file contains definitions Btl data base services.
*     The Device Database is used by the Management Entity to store link
*     keys and other information in a non-volatile way. It is also available
*		to the user application and maintain data about peer devices. Aplication can also
*		store its data field per remote BD addr in this DB.
*
*   AUTHOR:         Gili Friedman
*
\*******************************************************************************/

#ifndef __BTL_BMG_DDB_H
#define __BTL_BMG_DDB_H


/********************************************************************************
 *
 * Include files
 *
 *******************************************************************************/
#include "btl_bmg.h"

/********************************************************************************
 *
 * Definitions
 *
 *******************************************************************************/
#define BTL_BMG_DDB_INVALID_PSI_REP_MODE		((U8)0xFF)
#define BTL_BMG_DDB_INVALID_PSI_MODE			((U8)0xFF)

/********************************************************************************
 *
 * Types
 *
 *******************************************************************************/
typedef enum 
{
	BTL_BMG_DDB_NOTIFICATION_TYPE_RECORD_ADDED,
	BTL_BMG_DDB_NOTIFICATION_TYPE_RECORD_MODIFIED
} BtlBmgDdbNotificationType;

typedef void (*BtlBmgDdbNotificationCb)(BtlBmgDdbNotificationType notificationType, BtlBmgDeviceRecord *record);

/********************************************************************************
 *
 * Data Structures
 *
 *******************************************************************************/

/********************************************************************************
 *
 * Functions Prototypes
 *
 *******************************************************************************/

/*---------------------------------------------------------------------------
 * BTL_BMG_DDB_RegisterForNotifications()
 *
 *     Client registration for DDB notifications
 *
 *	Description
 *
 *		Allows a single client to register for DDB notifications. Calling with a null pointer revokes the current client
 *		registration.
 *
 * Parameters:
 *
 *     cbFunc [In] - Callback function
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Return this if operation was successful to
 *         indicate that other device database calls may be made.
 *
 *     BT_STATUS_IN_USE - A client already registered for notifications
 */
BtStatus BTL_BMG_DDB_RegisterForNotifications(BtlBmgDdbNotificationCb cbFunc);

/*---------------------------------------------------------------------------
 * BTL_BMG_DDB_Open()
 *
 *     Called to open the device database.
 *
 * Parameters:
 *     None
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Return this if operation was successful to
 *         indicate that other device database calls may be made.
 *
 *     BT_STATUS_FAILED - Return this if the operation failed. Overall
 *         stack initialization (EVM_Init) will fail as a result.
 */
BtStatus BTL_BMG_DDB_Open();


/*---------------------------------------------------------------------------
 * BTL_BMG_DDB_Close()
 *
 *     Called to close the device database.
 *
 */
BtStatus BTL_BMG_DDB_Close(void);


/*---------------------------------------------------------------------------
 * BTL_BMG_DDB_Flush()
 *
 *     Called to flush the device database to disk. This function 
 *     is called when new devices are added to the database. 
 *
 */
BtStatus BTL_BMG_DDB_Flush(void);


/*---------------------------------------------------------------------------
 * BTL_BMG_DDB_AddRecord()
 *
 *     Called to add or replace a record in the database.
 *     If the device record indicates a BD_ADDR that is already in the
 *     database, this function should replace the existing record.
 *
 * Parameters:
 *     record - Pointer to the record which is to be copied into the
 *         database. The structure should not be used by DDB after this call
 *         has returned.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Return this to indicate that the record
 *         was successfully written to the database.
 *
 *     BT_STATUS_FAILED - Return this to indicate that the record could
 *         not be written to the database.
 */
BtStatus BTL_BMG_DDB_AddRecord(const BtlBmgDeviceRecord *record);


/*---------------------------------------------------------------------------
 * BTL_BMG_DDB_FindRecord()
 *
 *     Called to copy a record out of the database. The record
 *     is identified by the BD_ADDR that represents the device.
 *
 * Parameters:
 *     bdAddr - 48-bit address that identifies the desired record.
 * 
 *     record - pointer to the record structure to fill with the
 *         specified record's information.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Return this if the record was found and copied
 *         into the "record" parameter.
 *
 *     BT_STATUS_FAILED - Returns this if the record could not be found
 *         or an error occurred while accessing non-volatile memory.
 */
BtStatus BTL_BMG_DDB_FindRecord(const BD_ADDR *bdAddr, BtlBmgDeviceRecord *record);


/*---------------------------------------------------------------------------
 * BTL_BMG_DDB_DeleteRecord()
 *
 *     Called to to remove a record from the database. The record
 *     is identified by the BD_ADDR that represents the device.
 * 
 * Parameters:
 *     bdAddr - 48-bit address contained in the record to delete.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Return this if the record was found and deleted.
 *
 *     BT_STATUS_FAILED - Return this if the record could not be found or
 *         deleted.
 */
BtStatus BTL_BMG_DDB_DeleteRecord(const BD_ADDR *bdAddr);

/*---------------------------------------------------------------------------
 * BTL_BMG_DDB_DeleteAllRecords()
 *
 *     Called to to remove all records from the database.
 *
 * Parameters:
 *     void.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Return this if all records were deleted.
 *
 *     BT_STATUS_FAILED - Return this if all records were not deleted.
 */
BtStatus BTL_BMG_DDB_DeleteAllRecords(void);


/*---------------------------------------------------------------------------
 * BTL_BMG_DDB_EnumDeviceRecords()
 *
 *     Called to to read a specific record from the device
 *     database. Records are stored with indexes between 0 and n-1 where n
 *     is the number of records in the database.
 *
 *     The stack calls this function to read device records from 0 to n-1
 *     in increasing sequence. The stack does not call other DDB functions
 *     during enumeration.
 *
 * Parameters:
 *     index - index of desired record. The first record is at index 0.
 *
 *     record - pointer to the record structure to fill with the
 *         specified record's information.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Return this if the record was located and copied
 *         into the "record" parameter.
 *
 *     BT_STATUS_FAILED - Return this if the record could not be found or
 *         copied. For example, BT_STATUS_FAILED should be returned if the
 *         specified index is invalid.
 */
BtStatus BTL_BMG_DDB_EnumDeviceRecords(U32 index, BtlBmgDeviceRecord* record);

/*---------------------------------------------------------------------------
 * BTL_BMG_DDB_InitRecord()
 *
 *	Initializes record. call this function before adding a new record to the DB - it resets all record values.
 *
 * Parameters:
 *     record - pointer to the record structure to init
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Operation ended successfuly
 *
 *     BT_STATUS_INVALID_PARAM - BtlBmgDeviceRecord is null.
 */
BtStatus BTL_BMG_DDB_InitRecord(BtlBmgDeviceRecord* record);

/*---------------------------------------------------------------------------
 * BTL_BMG_DDB_ClearSecurityInfo()
 *
 *	Clears security fields - link key, link key type, trust & trust per service
 *
 * Parameters:
 *     record - pointer to the record structure to init
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Operation ended successfuly
 *
 *     BT_STATUS_INVALID_PARAM - BtlBmgDeviceRecord is null.
 */
BtStatus BTL_BMG_DDB_ClearSecurityInfo(BtlBmgDeviceRecord* record);

/*---------------------------------------------------------------------------
 * BTL_BMG_DDB_GetNumOfDeviceRecords()
 *
 *	returns the number of devices in DB
 *
 * Parameters:
 *     answer[out] - number of records.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Operation ended successfuly
 *
 */
BtStatus BTL_BMG_DDB_GetNumOfDeviceRecords(U32 *answer);
 
BtStatus BTL_BMG_DDB_IsFull(BOOL *answer);


#endif /* __BTL_BMG_DDB_H */

