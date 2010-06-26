#ifndef __DDBH
#define __DDBH

/****************************************************************************
 *
 * File:
 *     $Workfile:ddb.h$ for iAnywhere Blue SDK, Version 2.1.2
 *     $Revision:26$
 *
 * Description:
 *     Definitions the API used by the stack to access a system-specific
 *     device database.
 * 
 * Copyright 1999-2005 Extended Systems, Inc.
 * Portions copyright 2005-2006 iAnywhere Solutions, Inc.
 * All rights reserved. All unpublished rights reserved.
 *
 * Unpublished Confidential Information of iAnywhere Solutions, Inc.  
 * Do Not Disclose.
 *
 * No part of this work may be used or reproduced in any form or by any 
 * means, or stored in a database or retrieval system, without prior written 
 * permission of iAnywhere Solutions, Inc.
 * 
 * Use of this work is governed by a license granted by iAnywhere Solutions,
 * Inc.  This work contains confidential and proprietary information of
 * iAnywhere Solutions, Inc. which is protected by copyright, trade secret,
 * trademark and other intellectual property rights.
 *
 ****************************************************************************/

#include "bttypes.h"
#include "me.h"


/*---------------------------------------------------------------------------
 * Device Database API layer
 *
 *     The Device Database is used by the Management Entity to store link
 *     keys and other information in a non-volatile way. This allows security
 *     information about a device to be used in later sessions, even if the
 *     device is switched off.
 *
 *     The database accessed by these calls must be maintained in
 *     non-volatile memory, such as flash RAM or a hard disk drive.
 */

/****************************************************************************
 *
 * Function Reference
 *
 ****************************************************************************/


#if (TI_CHANGES == XA_ENABLED)

/*---------------------------------------------------------------------------
 * DDB_Open()
 *
 *     Called by the stack to open the device database. This function is
 *     called during stack initialization, before any other DDB_ calls
 *     are made.
 *
 * Parameters:
 *     None.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Return this if operation was successful to
 *         indicate that other device database calls may be made.
 *
 *     BT_STATUS_FAILED - Return this if the operation failed. Overall
 *         stack initialization (EVM_Init) will fail as a result.
 */
BtStatus DDB_Open();

#else

/*---------------------------------------------------------------------------
 * DDB_Open()
 *
 *     Called by the stack to open the device database. This function is
 *     called during stack initialization, before any other DDB_ calls
 *     are made.
 *
 * Parameters:
 *     bdAddr - 48-bit address of the local radio.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Return this if operation was successful to
 *         indicate that other device database calls may be made.
 *
 *     BT_STATUS_FAILED - Return this if the operation failed. Overall
 *         stack initialization (EVM_Init) will fail as a result.
 */
BtStatus DDB_Open(const BD_ADDR *bdAddr);

#endif

/*---------------------------------------------------------------------------
 * DDB_Close()
 *
 *     Called by the stack to close the device database. This function is
 *     called during stack deinitialization.
 *
 * Returns:
 *     Ignored by the stack.
 */
BtStatus DDB_Close(void);


/*---------------------------------------------------------------------------
 * DDB_Flush()
 *
 *     Called by the stack to flush the device database to disk. This function 
 *     is called when new devices are added to the database. 
 *
 * Returns:
 *     Ignored by the stack.
 */
BtStatus DDB_Flush(void);


/*---------------------------------------------------------------------------
 * DDB_AddRecord()
 *
 *     Called by the stack to add or replace a record in the database.
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
BtStatus DDB_AddRecord(const BtDeviceRecord* record);


/*---------------------------------------------------------------------------
 * DDB_FindRecord()
 *
 *     Called by the stack to copy a record out of the database. The record
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
BtStatus DDB_FindRecord(const BD_ADDR *bdAddr, BtDeviceRecord* record);

#if TI_CHANGES == XA_ENABLED
/* Gili - this function should be merged with DDB_FindRecord. there should be one function that
returns whether key exists or not and another that returns the record.*/
BtStatus DDB_FindRecordRegardlessKeyInfo(const BD_ADDR *bdAddr, BtDeviceRecord* record);
#endif


/*---------------------------------------------------------------------------
 * DDB_DeleteRecord()
 *
 *     Called by the stack to remove a record from the database. The record
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
BtStatus DDB_DeleteRecord(const BD_ADDR *bdAddr);


#if TI_CHANGES == XA_ENABLED
/*---------------------------------------------------------------------------
 * DDB_DeleteAllRecords()
 *
 *     Called by the stack to remove all records from the database.
 *
 * Parameters:
 *     void.
 *
 * Returns:
 *     BT_STATUS_SUCCESS - Return this if all records were deleted.
 *
 *     BT_STATUS_FAILED - Return this if all records were not deleted.
 */
BtStatus DDB_DeleteAllRecords(void);
#endif /* TI_CHANGES == XA_ENABLED */


/*---------------------------------------------------------------------------
 * DDB_EnumDeviceRecords()
 *
 *     Called by the stack to read a specific record from the device
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
BtStatus DDB_EnumDeviceRecords(I16 index, BtDeviceRecord* record);

#endif /* __DDBH */
